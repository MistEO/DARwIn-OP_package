#include "objectdetector.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

cv::VideoCapture ObjectDetector::capture(0);

ObjectDetector::ObjectDetector(const std::string & obj_name, int imshow_flag,
	bool mixed_hue, double scale)
	:cap_width(capture.get(CV_CAP_PROP_FRAME_WIDTH) * scale),
	cap_height(capture.get(CV_CAP_PROP_FRAME_HEIGHT) * scale),
	obj_name(obj_name), show_flag(imshow_flag),
	mixed_flag(mixed_hue), scale(scale),
	lower_color(0, 0, 0, 0), upper_color(180, 180, 255, 255),
	rect(cap_width / 2, cap_height / 2, 0, 0)
{
	assert(capture.isOpened());
	load();
	cv::namedWindow(obj_name, CV_WINDOW_NORMAL);
}

ObjectDetector::~ObjectDetector()
{
}

void ObjectDetector::load()
{
	cv::FileStorage fs(obj_name, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::clog << "Cannot read file: " << obj_name << std::endl;
		return;
	}
	cv::FileNode fn_lower = fs["lower_color"],
		fn_upper = fs["upper_color"];
	cv::FileNodeIterator it_lower = fn_lower.begin(),
		it_upper = fn_upper.begin();
	for (int i = 0; i != 4; ++i, ++it_lower, ++it_upper) {
		if (fn_lower.size() == 3 && i == 1) {
			--it_lower;
			--it_upper;
			continue;
		}
		lower_color[i] = static_cast<int>(*it_lower);
		upper_color[i] = static_cast<int>(*it_upper);
	}
	fs.release();
}

void ObjectDetector::save()
{
	cv::FileStorage fs(obj_name, cv::FileStorage::WRITE);
	fs << "lower_color" << "[" << lower_color[0] << lower_color[1] << lower_color[2] << lower_color[3] << "]";
	fs << "upper_color" << "[" << upper_color[0] << upper_color[1] << upper_color[2] << upper_color[3] << "]";
	fs.release();
}

void ObjectDetector::refresh_capture(int times)
{
	cv::Mat temp_image;
	for (int i = 0; i != times; ++i) {
		capture >> temp_image;
	}
}

inline bool rect_area_cmp(cv::Rect& lhs, const cv::Rect& rhs)
{
	return lhs.area() < rhs.area();
}

int ObjectDetector::process_by_color(int wait_msec, int rect_filter)
{
	using namespace cv;

	Mat source_image;
	capture >> source_image;
	if (source_image.empty()) {
		std::cerr << "The source image is empty" << std::endl;
		return -1;
	}

	Mat resize_image;
	resize(source_image, resize_image,
		Size(source_image.cols * scale, source_image.rows * scale));	//按比例缩放图像

	Mat hsv_image;
	cvtColor(resize_image, hsv_image, CV_BGR2HSV);	//转换为HSV色彩空间
	Mat binary_image;
	inRange(hsv_image, get_lower_color(), get_upper_color(), binary_image);	//按范围二值化

	if (mixed_flag) {
		Mat binary_image2;
		inRange(hsv_image, get_lower_color(true), get_upper_color(true), binary_image2);
		addWeighted(binary_image2, 1, binary_image, 1, 0, binary_image);
	}

	morphologyEx(binary_image, binary_image, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//开操作降噪
	morphologyEx(binary_image, binary_image, CV_MOP_OPEN,
		getStructuringElement(MORPH_RECT, Size(10, 10)));	//闭操作降噪

	using std::vector;

	vector<vector<Point> > all_contours;
	vector<Vec4i> hierarchy;
	Mat contours_image = binary_image.clone();
	findContours(contours_image, all_contours, hierarchy,
		CV_RETR_TREE, CV_CHAIN_APPROX_NONE);		//查找轮廓
	vector<Rect> all_bound;
	for (vector<vector<Point> >::iterator i = all_contours.begin();
		i != all_contours.end(); ++i) {
		Rect temp = boundingRect(Mat(*i));	//从轮廓获取矩形
		if (rect_filter & Vertical) {		//过滤掉不是竖立着的
			if (temp.width > temp.height)
				continue;
		}
		all_bound.push_back(temp);
	}
	rect = Rect(cap_width / 2, cap_height / 2, 0, 0);
	vector<cv::Rect>::iterator biggest_rect_iter
		= std::max_element(all_bound.begin(), all_bound.end(), rect_area_cmp);	//获取面积最大的矩形的迭代器
	if (biggest_rect_iter != all_bound.end()) {		//若为.end()即没有最大的，则图像中没有轮廓
		rect = *biggest_rect_iter;
	}

	//显示的窗口
	if (show_flag != NotShow) {
		Mat show_image = Mat::zeros(resize_image.size(), resize_image.type());
		if (show_flag & ShowSource) {	//显示原图片
			show_image = resize_image.clone();
		}
		if (show_flag & ShowBinary) {	//显示二值化部分
			Mat bgr_binary_image;
			cvtColor(binary_image, bgr_binary_image, CV_GRAY2BGR);	//转换为三通道，以合并
			const double alpha = 0.5, beta = 0.5, gamma = 0.0;
			addWeighted(show_image, alpha, bgr_binary_image, beta, gamma, show_image);	//线性叠加
		}
		if (show_flag & ShowDrawing) {	//划线和矩形
			line(show_image, cv::Point(show_image.cols / 2, 0),
				Point(show_image.cols / 2, show_image.rows),
				Scalar(255, 0, 0), 2);		//画图像中轴线
			for (vector<Rect>::iterator i = all_bound.begin(); i != all_bound.end(); ++i) {	//画所有矩形
				rectangle(show_image, *i, cv::Scalar(0, 0, 0), 1);
			}
			line(show_image,
				Point(rect.x + rect.width / 2, rect.y),
				Point(rect.x + rect.width / 2, rect.y + rect.height),
				Scalar(0, 0, 255), 2);		//画最大矩形中线
			rectangle(show_image, rect, get_avg_color(), 2);	//画最大矩形
		}
		cv::imshow(obj_name, show_image);
	}
	return waitKey(wait_msec);
}

void ObjectDetector::adjust_color()
{
	using cv::createTrackbar;

	const std::string adjust_window_name = obj_name + " - Adjust Color";
	cv::namedWindow(adjust_window_name, cv::WINDOW_AUTOSIZE);
	createTrackbar("Low H", adjust_window_name, &lower_color[0], 180);
	createTrackbar("Up H", adjust_window_name, &upper_color[0], 180);
	if (mixed_flag) {
		createTrackbar("Low H 2", adjust_window_name, &lower_color[1], 180);
		createTrackbar("Up H 2", adjust_window_name, &upper_color[1], 180);
	}
	createTrackbar("Low S", adjust_window_name, &lower_color[2], 255);
	createTrackbar("Up S", adjust_window_name, &upper_color[2], 255);
	createTrackbar("Low V", adjust_window_name, &lower_color[3], 255);
	createTrackbar("Up V", adjust_window_name, &upper_color[3], 255);

	int c = 0;
	while (c != 27) {
		c = process_by_color(40);
	}
	save();
	cv::destroyWindow(adjust_window_name);
}

cv::Scalar ObjectDetector::get_lower_color(bool second_hue)
{
	return cv::Scalar(lower_color[second_hue], lower_color[2], lower_color[3]);
}

cv::Scalar ObjectDetector::get_upper_color(bool second_hue)
{
	return cv::Scalar(upper_color[second_hue], upper_color[2], upper_color[3]);
}

cv::Scalar avg(const cv::Scalar & lhs, const cv::Scalar & rhs)
{
	return cv::Scalar((lhs[0] + rhs[0]) / 2, (lhs[1] + rhs[1]) / 2, (lhs[0] + rhs[1]) / 2);
}

cv::Scalar ObjectDetector::get_avg_color()
{
	cv::Scalar avg_temp = avg(get_lower_color(), get_upper_color());
	if (mixed_flag) //混合色求平均，懒得写了凑合用吧-。-
		avg_temp[0] = 0; 
	return avg_temp;
}

int ObjectDetector::width() const
{
	return rect.width * scale;
}

int ObjectDetector::height() const
{
	return rect.height * scale;
}

int ObjectDetector::postion() const
{
	return x_axis() * scale;
}

int ObjectDetector::x_axis() const
{
	return rect.x + (rect.width / 2);
}

int ObjectDetector::y_axis() const
{
	return rect.br().y;
}

std::pair<int, int> ObjectDetector::anchor_point() const
{
	return std::pair<int, int>
		(x_axis() * scale, y_axis() * scale);
}

bool ObjectDetector::empty() const
{
	return rect.area() == 0;
}

int & ObjectDetector::imshow_flag()
{
	return show_flag;
}

const std::string ObjectDetector::object_name() const
{
	return obj_name;
}
