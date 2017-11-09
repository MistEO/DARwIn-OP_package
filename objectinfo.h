#pragma once

#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <set>

//比较cv::Rect面积
struct RectCompare {
	 bool operator()(const cv::Rect& lhs, const cv::Rect& rhs)
	{
		return lhs.area() > rhs.area();
	}
};

//图像信息类，保存HSV值
//若为红色类似色，Hue值需要取0或180附近共两部分，则可通过设置mixed=true，同时保存两端Hue值
class ObjectInfo {
public:
	explicit ObjectInfo(bool mixed_flag = false, int count = 1);

	//获取颜色下限、上限
	cv::Scalar get_lower(bool second_hue = false);
	cv::Scalar get_upper(bool second_hue = false);
	//获取平均色，可用于画框之类的，为BGR值
	cv::Scalar get_average();

	//混合选项
	bool mixed;
	//物体数量
	int count;

	//下限颜色和上限颜色
	//[0]:H值 [1]:H2第二部分，若不是红色则为0 [2]:S [3]:V
	cv::Vec4i lower, upper;
	//包含该色的所有矩形
	std::set<cv::Rect, RectCompare> rect_set;
};

