#pragma once

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>

#include <string>
#include <map>
#include <utility>

#include "objectinfo.h"

//��������
enum RectAttr {
	LeftX, RightX, TopY, BottomY, CenterX, CenterY,
	Height, Width, Area
};

class Detector {
public:
	//��ȡ��̬����
	static Detector & ins();
	~Detector();
	
	//����ͼ�����ţ�������Сͼ����������ܣ�����refresh��������
	void set_scale(double scale);
	//�����µ�׷��ɫ
	void insert(const std::string & name,
		bool mixed_flag = false, int count = 1);
	//ɾ��ĳ��׷��ɫ
	void erase(const std::string & name);
	//�������
	void clear();
	
	//����һ֡ͼ��
	int refresh(uint times = 1);
	//����ĳ��׷��ɫ
	void process(const std::string & object_name, const std::string & window_name = "");
	//��ȡ��������ֵ
	int get_rect_attr(const std::string & object_name, const int order, RectAttr attr) const;
	//����ĳ��׷��ɫHSVֵ
	void adjust_color(const std::string & object_name, int confirm_keycode = 13, int cancel_keycode = 27);
	//��ʾ����
	void show();
	
private:
	//��ȡ�ͱ����HSVֵ��YMAL�ļ�
	void load(const std::string & name);
	void save(const std::string & name);

	//��̬�������ܾ����캯��
	Detector();
	static Detector unique_ins;

	cv::VideoCapture capture;
	//���ƺ���ɫ��Ϣ��ֵ��
	std::map<std::string, ObjectInfo> object_map;
	//���ű���
	double scale;
	//ԭͼ�����ź�ͼ��hsvͼ����ʾ��ͼ��
	cv::Mat source_image, scale_image, hsv_image, show_image;
};