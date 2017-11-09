#pragma once

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

#include <string>
#include <map>
#include <utility>

#include "objectinfo.h"

//矩形属性
enum RectAttr {
	LeftX, RightX, TopY, BottomY, CenterX, CenterY,
	Height, Width, Area
};

class Detector {
public:
	//获取静态单例
	static Detector & ins();
	~Detector();
	
	//设置图像缩放，可以缩小图像以提高性能，调用refresh函数更新
	void set_scale(double scale);
	//插入新的追踪色
	void insert(const std::string & name,
		bool mixed_flag = false, int count = 1);
	//删除某个追踪色
	void erase(const std::string & name);
	//清空所有
	void clear();
	
	//更新一帧图像
	int refresh(uint times = 1);
	//处理某个追踪色
	void process(const std::string & object_name, const std::string & window_name = "");
	//获取矩形属性值
	int get_rect_attr(const std::string & object_name, const int order, RectAttr attr) const;
	//调整某个追踪色HSV值
	void adjust_color(const std::string & object_name, int confirm_keycode = 13, int cancel_keycode = 27);
	//显示窗口
	void show();
	
private:
	//读取和保存的HSV值，YMAL文件
	void load(const std::string & name);
	void save(const std::string & name);

	//静态单例，拒绝构造函数
	Detector();
	static Detector unique_ins;

	cv::VideoCapture capture;
	//名称和颜色信息键值对
	std::map<std::string, ObjectInfo> object_map;
	//缩放比例
	double scale;
	//原图像、缩放后图像、hsv图像、显示的图像
	cv::Mat source_image, scale_image, hsv_image, show_image;
};