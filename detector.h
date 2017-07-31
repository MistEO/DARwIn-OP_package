#pragma once

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

#include <string>
#include <map>
#include <utility>

#include "objectinfo.h"

class Detector {
public:
	static Detector & ins();
	~Detector();
	
	void set_scale(double scale);
	
	void insert(const std::string & name,
		int show_flag = NotShow, bool mixed_flag = false);
	void erase(const std::string & name);
	void clear();
	
	void refresh(uint times = 1);
	void process(const std::string & object_name);
	void show();
	
private:
	Detector();
	static Detector unique_ins;
	
	cv::VideoCapture capture;
	std::map<std::string, ObjectInfo> object_map;
	
	double scale;
	
	cv::Mat source_image, scale_image, hsv_image, show_image;
};