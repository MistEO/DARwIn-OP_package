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
		bool mixed_flag = false, int count = 1, int show_flag = ShowDrawing);
	void erase(const std::string & name);
	void clear();
	
	int refresh(uint times = 1);
	void process(const std::string & object_name, const std::string & window_name = "");
	void adjust_color(const std::string & object_name, int confirm_keycode = 13, int cancel_keycode = 27);
	void show();
	
private:
	void load(const std::string & name);
	void save(const std::string & name);

	Detector();
	static Detector unique_ins;
	
	cv::VideoCapture capture;
	std::map<std::string, ObjectInfo> object_map;
	
	double scale;
	
	cv::Mat source_image, scale_image, hsv_image, show_image;
};