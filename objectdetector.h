#pragma once

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

#include <string>

//显示窗口标识
enum ShowImage {
	NotShow = 0,
	ShowSource = 1,
	ShowBinary = 2,
	ShowDrawing = 4,
	ShowAll = ShowSource | ShowBinary | ShowDrawing
};

//矩形过滤器选项
enum RectFilter {
	NotFilter = 0,
	Vertical = 1,
};

class ObjectDetector
{
public:
	//构造函数，参数为名称、显示窗口标识
	ObjectDetector(const std::string & obj_name, int imshow_flag = NotShow);
	//析构函数，会保存颜色的值为文件
	~ObjectDetector();

	//刷新视频画面，参数为刷新次数
	void refresh_capture(int times = 2);

	//视频宽度、高度
	static const double cap_width;
	static const double cap_height;

	//调整颜色，析构函数会自动保存
	void adjust_color();
	//按颜色处理图像，参数为处理间隔时间（毫秒）
	int process_by_color(int wait_msec, int rect_filter = NotFilter);

	//返回上下限颜色标量（类似容器，看不懂先忽略）
	cv::Scalar & lower_color();
	cv::Scalar & upper_color();

	//返回物体的宽度、高度、位置、是否存在
	int width() const;
	int height() const;
	int postion() const;
	bool empty() const;
	
	//获取显示窗口标识，同时也可以直接设置（obj.imshow_flag() = ShowBinary)
	int & imshow_flag();
	//返回物体名
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

