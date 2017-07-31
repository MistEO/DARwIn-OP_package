#include "detector.h"

const char * WindowName = "Detector";

Detector Detector::unique_ins;

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
}

void Detector::insert(const std::string & name,
	int show_flag, bool mixed_flag)
{
	object_map[name] = ObjectInfo(show_flag, mixed_flag);
}

void Detector::erase(const std::string & name)
{
	object_map.erase(name);
}

void Detector::clear()
{
	object_map.clear();
}

void Detector::refresh(uint times)
{
	cv::Size capture_size(capture.get(CV_CAP_PROP_FRAME_WIDTH), capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	for (int i = 0; i != times; ++i) {
		cv::waitKey(40);
		capture >> source_image;
	}
	cv::resize(source_image, scale_image, capture_size);
	cv::cvtColor(scale_image, hsv_image, CV_BGR2HSV);
}

void Detector::process(const std::string & object_name)
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
		getStructuringElement(MORPH_RECT, Size(10 * scale, 10 * scale)));	//¿ª²Ù×÷½µÔë
	morphologyEx(binary_image, binary_image, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10 * scale, 10 * scale)));	//±Õ²Ù×÷½µÔë

	using std::vector;
	vector<vector<Point> > all_contours;
	vector<Vec4i> hierarchy;
	Mat contours_image = binary_image.clone();
	findContours(contours_image, all_contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE);		//²éÕÒÂÖÀª
	for (vector<vector<Point> >::iterator i = all_contours.begin();
		i != all_contours.end(); ++i) {
		oinfo.rect_set.insert(boundingRect(Mat(*i)));
	}
}