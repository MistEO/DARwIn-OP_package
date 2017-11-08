#include "objectinfo.h"
#include <iostream>

ObjectInfo::ObjectInfo(bool mixed_flag, int count, int show_flag)
	: mixed(mixed_flag), count(count), show(show_flag),
	lower(0, 0, 0, 0), upper(181, 181, 256, 256)
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

cv::Scalar ObjectInfo::get_average()
{
	cv::Scalar avg_hsv;
	if (!mixed) {
		avg_hsv[0] = (get_lower()[0] + get_upper()[0]) / 2;
	}
	else {
		avg_hsv[0] = ((get_lower()[0] + get_upper()[0]) / 2 + (get_lower(true)[0] + get_upper(true)[0]) / 2 + 180) / 2;
		if (avg_hsv[0] > 180) {
			avg_hsv[0] -= 180;
		}
	}
	avg_hsv[1] = (get_lower()[1] + get_upper()[1]) / 2;
	avg_hsv[2] = (get_lower()[2] + get_upper()[2]) / 2;

	//不知道怎么把HSV的Scalar转换为BGR的，只能用Mat转一下了=。=
	cv::Mat hsv_mat(1, 1, CV_8UC3, avg_hsv), bgr_mat;
	cv::cvtColor(hsv_mat, bgr_mat, CV_HSV2BGR);
	return bgr_mat.at<cv::Vec3b>(0, 0);
}