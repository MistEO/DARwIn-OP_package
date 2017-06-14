#ifndef MOTION_H
#define MOTION_H

#include <LinuxDARwIn.h>
#include <DARwIn.h>
#include <LinuxCM730.h>

class Motion
{
public:
	~Motion();
	static Motion& ins();	//返回唯一静态单例

	void walk_start();		//开始行走
	//设置行走速度，三个参数分别为直行振幅（速度），左右振幅，缓慢加速时间（为0即不缓慢加速）
	void walk(int x_move, int a_move, int msec = 0);
	//检测跌倒，若跌倒会自动爬起，返回true；没跌倒返回false
	bool fall_up();
	//移动头部，三个参数分别为左右移动角度，上下移动角度，绝对/相对移动（true为绝对移动，false为相对移动）
	void head_move(int x, int y, bool home = true);
	//根据序号进行动作
	void action(int index);

private:
	Motion();
	minIni* ini;
	Robot::LinuxCM730 linux_cm730;
	Robot::CM730 cm730;
	Robot::LinuxMotionTimer *motion_timer;

	static Motion unique_instance;
	
};


#endif // !MOTION_H

