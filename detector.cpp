#include "detector.h"
#include <iostream>

const char * WindowName = "Detector";
const char * YAMLPath = "hsv-value.yaml";

Detector Detector::unique_ins;

Detector & Detector::ins()
{
	return unique_ins;
}

Detector::Detector()
	: capture(0), scale(1.0)
{
	assert(capture.isOpened());
	cv::namedWindow(WindowName, CV_WINDOW_NORMAL);
}

Detector::~Detector()
{
	capture.release();
}

void Detector::set_scale(double scale)
{
	this->scale = scale;
	scale_image.release();
	hsv_image.release();
	show_image.release();
}

void Detector::insert(const std::string & name,
	int count, int show_flag, bool mixed_flag)
{
	object_map[name] = ObjectInfo(count, show_flag, mixed_flag);
	load(name);
}

void Detector::erase(const std::string & name)
{
	object_map.erase(name);
}

void Detector::clear()
{
	object_map.clear();
}

int Detector::refresh(uint times)
{
	cv::Size capture_size(
		capture.get(CV_CAP_PROP_FRAME_WIDTH),
		capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	int c = 0;
	for (int i = 0; i != times; ++i) {
		c = cv::waitKey(40);
		capture >> source_image;
	}
	cv::resize(source_image, scale_image, capture_size);
	cv::cvtColor(scale_image, hsv_image, CV_BGR2HSV);
	return c;
}

void Detector::process(const std::string & object_name, const std::string & window_name)
{
	using namespace cv;
	ObjectInfo & oinfo = object_map[object_name];
	Mat binary_image;
	inRange(hsv_image, oinfo.get_lower(), oinfo.get_upper(), binary_image);
	if (oinfo.mixed) {
		Mat binary_image2;
		inRange(hsv_image, oinfo.get_lower(), oinfo.get_upper(), binary_image2);
		addWeighted(binary_image2, 1, binary_image, 1, 0, binary_image);
	}

	morphologyEx(binary_image, binary_image, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10 * scale, 10 * scale)));	//开操作降噪
	morphologyEx(binary_image, binary_image, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10 * scale, 10 * scale)));	//闭操作降噪

	using std::vector;
	vector<vector<Point> > all_contours;
	vector<Vec4i> hierarchy;
	Mat contours_image = binary_image.clone();
	findContours(contours_image, all_contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE);		//查找轮廓
	oinfo.rect_set.clear();
	for (vector<vector<Point> >::iterator i = all_contours.begin();
		i != all_contours.end(); ++i) {
		oinfo.rect_set.insert(boundingRect(Mat(*i)));
	}

	if (!window_name.empty()) {
		Mat show_image = Mat::zeros(scale_image.size(), scale_image.type());
		show_image = scale_image.clone();
		Mat bgr_binary_image;
		cvtColor(binary_image, bgr_binary_image, CV_GRAY2BGR);	//转换为三通道，以合并
		const double alpha = 0.5, beta = 0.5, gamma = 0.0;
		addWeighted(show_image, alpha, bgr_binary_image, beta, gamma, show_image);	//线性叠加
		line(show_image, cv::Point(show_image.cols / 2, 0),
			Point(show_image.cols / 2, show_image.rows),
			Scalar(255, 0, 0), 2);		//画图像中轴线
		if (!oinfo.rect_set.empty()) {
			rectangle(show_image, *oinfo.rect_set.begin(), oinfo.get_average(), 2);	//画最大矩形
			for (std::set<cv::Rect, RectCompare>::iterator i = oinfo.rect_set.begin();
				i != oinfo.rect_set.end(); ++i) {	//画所有矩形
				rectangle(show_image, *i, cv::Scalar(0, 0, 0), 1);
			}
		}
		imshow(window_name, show_image);
	}
}

void Detector::adjust_color(const std::string & object_name, int confirm_keycode, int cancel_keycode)
{
	using cv::createTrackbar;

	ObjectInfo & oinfo = object_map[object_name];
	const std::string adjust_window_name = object_name + " - Adjust Color";
	cv::namedWindow(adjust_window_name, cv::WINDOW_AUTOSIZE);
	createTrackbar("Low H", adjust_window_name, &oinfo.lower[0], 181);
	createTrackbar("Up H", adjust_window_name, &oinfo.upper[0], 181);
	if (oinfo.mixed) {
		createTrackbar("Low H 2", adjust_window_name, &oinfo.lower[1], 181);
		createTrackbar("Up H 2", adjust_window_name, &oinfo.upper[1], 181);
	}
	createTrackbar("Low S", adjust_window_name, &oinfo.lower[2], 256);
	createTrackbar("Up S", adjust_window_name, &oinfo.upper[2], 256);
	createTrackbar("Low V", adjust_window_name, &oinfo.lower[3], 256);
	createTrackbar("Up V", adjust_window_name, &oinfo.upper[3], 256);

	int c = 0;
	while (c != confirm_keycode && c != cancel_keycode) {
		c = refresh();
		process(object_name, adjust_window_name);
	}
	if (c == confirm_keycode) {
		save(object_name);
	}

	cv::destroyWindow(adjust_window_name);
}

void Detector::show()
{
	show_image = scale_image.clone();
	for (std::map<std::string, ObjectInfo>::iterator i = object_map.begin();
		i != object_map.end(); ++i) {

		int rectangle_count = 0;
		for (std::set<cv::Rect, RectCompare>::iterator j = i->second.rect_set.begin();
			i->second.show & ShowDrawing && j != i->second.rect_set.end() && rectangle_count != i->second.count;
			++j, ++rectangle_count) {
			cv::rectangle(show_image, *j, i->second.get_average());
		}
	}
	cv::imshow(WindowName, show_image);
}

void Detector::load(const std::string & name)
{
	cv::FileStorage fs(YAMLPath, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::clog << "Cannot read file: " << YAMLPath << std::endl;
		return;
	}
	ObjectInfo & oinfo = object_map[name];
	cv::FileNode fn_lower = fs[name + "-lower"],
		fn_upper = fs[name + "-upper"];
	cv::FileNodeIterator it_lower = fn_lower.begin(),
		it_upper = fn_upper.begin();
	for (int i = 0; i != 4; ++i, ++it_lower, ++it_upper) {
		oinfo.lower[i] = static_cast<int>(*it_lower);
		oinfo.upper[i] = static_cast<int>(*it_upper);
	}
	fs.release();
}

void Detector::save(const std::string & name)
{
	cv::FileStorage fs(YAMLPath, cv::FileStorage::WRITE);
	ObjectInfo & oinfo = object_map[name];
	fs << name + "-lower" << "[" << oinfo.lower[0] << oinfo.lower[1] << oinfo.lower[2] << oinfo.lower[3] << "]";
	fs << name + "-upper" << "[" << oinfo.upper[0] << oinfo.upper[1] << oinfo.upper[2] << oinfo.upper[3] << "]";
	fs.release();
}