#include "objectdetector.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

cv::VideoCapture ObjectDetector::capture(0);
const double ObjectDetector::cap_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
const double ObjectDetector::cap_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

ObjectDetector::ObjectDetector(const std::string & obj_name, int imshow_flag)
	:obj_name(obj_name), show_flag(imshow_flag),
	lower_color(0, 0, 0), upper_color(180, 255, 255),
	rect(cap_width / 2, cap_height / 2, 0, 0)
{
	assert(capture.isOpened());
	load();
}

ObjectDetector::~ObjectDetector()
{
	save();
}

void ObjectDetector::load()
{
	cv::FileStorage fs(obj_name, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::clog << "Cannot read file: " << obj_name << std::endl;
		return;
	}
	fs["lower_color"] >> lower_color;
	fs["upper_color"] >> upper_color;
	fs.release();
}

void ObjectDetector::save()
{
	cv::FileStorage fs(obj_name, cv::FileStorage::WRITE);
	fs << "lower_color" << "[" << lower_color[0] << lower_color[1] << lower_color[2] << "]";
	fs << "upper_color" << "[" << upper_color[0] << upper_color[1] << upper_color[2] << "]";
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

	Mat hsv_image;
	cvtColor(source_image, hsv_image, CV_BGR2HSV);	//转换为HSV色彩空间
	Mat binary_image;
	inRange(hsv_image, get_lower_color(), get_upper_color(), binary_image);	//按范围二值化
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
		Mat show_image = Mat::zeros(source_image.size(), source_image.type());
		if (show_flag & ShowSource) {	//显示原图片
			show_image = source_image.clone();
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
			rectangle(show_image, rect, cv::Scalar(255, 255, 255), 2);	//画最大矩形
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
	createTrackbar("Low S", adjust_window_name, &lower_color[1], 255);
	createTrackbar("Up S", adjust_window_name, &upper_color[1], 255);
	createTrackbar("Low V", adjust_window_name, &lower_color[2], 255);
	createTrackbar("Up V", adjust_window_name, &upper_color[2], 255);
}

cv::Scalar ObjectDetector::get_lower_color()
{
	return lower_color;
}

cv::Scalar ObjectDetector::get_upper_color()
{
	return upper_color;
}

int ObjectDetector::width() const
{
	return rect.width;
}

int ObjectDetector::height() const
{
	return rect.height;
}

int ObjectDetector::postion() const
{
	return rect.tl().x + (rect.width / 2);
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
