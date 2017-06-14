#include "motion.h"
#include "objectdetector.h"
#include <iostream>
#include <signal.h>

void got_signal(int);

int main(int argc, char ** argv)
{
	//设置各种退出信号，看不懂先照抄，还有got_signal函数及其声明一起复制下来
	signal(SIGABRT, got_signal);
	signal(SIGTERM, got_signal);
	signal(SIGQUIT, got_signal);
	signal(SIGINT, got_signal);

	const int x_speed = 30;
	const int speed_up_msec = 200;

	//定义一个ObjectDetector类的对象，负责视觉追踪，第一个参数为名字，第二个参数是显示哪些窗口，可以不填
	ObjectDetector obj("test", ShowBinary | ShowSource | ShowDrawing);
	//调整颜色，会自动保存为文件，下次会自动读取
	obj.adjust_color();

	getchar();
	//开始行走（原地踏步）
	Motion::ins().walk_start();
	//设置行走速度，第一个参数是直行速度，第二个是左右速度（左正右负），第三个参数为加速所用时间（毫秒），可以不填
	Motion::ins().walk(x_speed, 0, speed_up_msec);
	while (true) {
		//处理一帧图像，参数为延时（毫秒）
		obj.process_by_color(10);
		//通过postion成员函数可以获取物体的位置，width成员函数可以获取物体的宽度，更多函数见objectdetector.h文件
		std::cout << "Postion: " << obj.postion() << " Width: " << obj.width() << std::endl;
		//检测跌倒，并自动爬起来，返回true
		if (Motion::ins().fall_up()) {
			//爬起来之后机器人是不动的，重新开始行走并设置速度
			Motion::ins().walk_start();
			Motion::ins().walk(x_speed, 0, speed_up_msec);
		}
	}
	return 0;
}

void got_signal(int)
{
	exit(0);
}
