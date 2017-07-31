#pragma once

#include <opencv2/core/core.hpp>
#include <string>
#include <set>

//显示窗口标识
enum ShowImage {
	NotShow = 0,
	DetachShow = 1,
	ShowBinary = 2,
	ShowDrawing = 4,
};

struct RectCompare {
	 bool operator()(const cv::Rect& lhs, const cv::Rect& rhs)
	{
		return lhs.area() > rhs.area();
	}
};

class ObjectInfo {
public:
	explicit ObjectInfo(int count = 1, int show_flag = ShowDrawing, bool mixed_flag = false);

	cv::Scalar get_lower(bool second_hue = false);
	cv::Scalar get_upper(bool second_hue = false);
	cv::Scalar get_average();

	int count;
	int show;
	bool mixed;
	cv::Vec4i lower, upper;
	std::set<cv::Rect, RectCompare> rect_set;
};

