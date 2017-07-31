#include "objectinfo.h"

ObjectInfo::ObjectInfo(int show_flag, bool mixed_flag)
	: show(show_flag), mixed(mixed_flag),
	rect_set()
{
	
}

cv::Scalar ObjectInfo::get_lower(bool second_hue)
{
	return cv::Scalar(lower[second_hue], lower[2], lower[3]);
}

cv::Scalar ObjectInfo::get_upper(bool second_hue)
{
	return cv::Scalar(upper[second_hue], upper[2], upper[3]);
}

cv::Scalar average(const cv::Scalar & lhs, const cv::Scalar & rhs)
{
	return cv::Scalar((lhs[0] + rhs[0]) / 2, (lhs[1] + rhs[1]) / 2, (lhs[0] + rhs[1]) / 2);
}

cv::Scalar ObjectInfo::get_average()
{
	cv::Scalar avg_temp = average(get_lower(), get_upper());
	if (mixed) //混合色求平均，懒得写了凑合用吧-。-
		avg_temp[0] = 0; 
	return avg_temp;
}