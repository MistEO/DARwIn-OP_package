#ifndef MOTION_H
#define MOTION_H

#include <LinuxDARwIn.h>
#include <DARwIn.h>
#include <LinuxCM730.h>

class Motion
{
public:
	~Motion();
	static Motion& ins();

	void walk_start();
	void walk(int x_move, int a_move, int msec = 0);
	bool fall_up();
	void head_move(int x, int y = 40, bool home = true);
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

