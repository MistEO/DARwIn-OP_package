#pragma once

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

#include <string>

enum ShowImage {
	NotShow = 0,
	ShowSource = 1,
	ShowBinary = 2,
	ShowDrawing = 4
};

class ObjectDetector
{
public:
	ObjectDetector(const std::string & obj_name, int imshow_flag = NotShow);
	~ObjectDetector();

	void refresh_capture(int times = 2);

	static const int cap_width;
	static const int cap_height;

	void adjust_color();
	int process_by_color(int wait_msec);

	cv::Scalar & lower_color();
	cv::Scalar & upper_color();

	int width() const;
	int height() const;
	int postion() const;
	
	int & imshow_flag();
	const std::string object_name() const;

private:
	void load();
	void save();

	static cv::VideoCapture capture;
	
	const std::string obj_name;
	int show_flag;
	
	cv::Scalar lower_limit_color,
		upper_limit_color;
	int lower_hue, lower_saturation, lower_value,
		upper_hue, upper_saturation, upper_value;
	cv::Rect rect;
	
};

