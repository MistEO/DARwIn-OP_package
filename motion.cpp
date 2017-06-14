#include "motion.h"
#include <unistd.h>
#include <cassert>
#include <cmath>

const char * INI_FILE_PATH = "/darwin/Data/config.ini";
const char * U2D_DEV_NAME = "/dev/ttyUSB0";
const char * MOTION_FILE_PATH = "/darwin/Data/motion_4096.bin";

#define NDEBUG

Motion Motion::unique_instance;

Motion& Motion::ins()
{
	return unique_instance;
}

Motion::Motion() :
	ini(new minIni(INI_FILE_PATH)),
	linux_cm730(U2D_DEV_NAME),
	cm730(&linux_cm730)
{
	using namespace Robot;
	Action::GetInstance()->LoadFile(const_cast<char*>(MOTION_FILE_PATH));
	
	assert(MotionManager::GetInstance()->Initialize(&cm730));
	Walking::GetInstance()->Initialize();
	MotionManager::GetInstance()->LoadINISettings(ini);
	Walking::GetInstance()->LoadINISettings(ini);

	MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
	MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
	MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());

	motion_timer = new LinuxMotionTimer(MotionManager::GetInstance());
	motion_timer->Start();
	MotionManager::GetInstance()->SetEnable(true);
	Action::GetInstance()->m_Joint.SetEnableBody(true, true);
	//Slowly stand up 
	Action::GetInstance()->Start(9);
	while(Action::GetInstance()->IsRunning() == true)
		usleep(8000);

	Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
	Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
	Head::GetInstance()->MoveByAngle(0, 60);
}

Motion::~Motion()
{
	using namespace Robot;
	Walking::GetInstance()->Stop();
	while (Walking::GetInstance()->IsRunning())
		usleep(8 * 1000);
	MotionManager::GetInstance()->RemoveModule((MotionModule*)Walking::GetInstance());
	Action::GetInstance()->Start(9);
	while (Action::GetInstance()->IsRunning())
		usleep(8 * 1000);
	delete motion_timer;		
	delete ini;
}

void Motion::walk_start()
{
	Robot::Walking::GetInstance()->Start();
}

void Motion::walk(int x_move, int a_move, int msec)
{
	#ifndef NDEBUG
		printf("Walking: X_Move:%d, A_Move:%d\n", x_move, a_move);
	#endif
	if (msec == 0) {
		Robot::Walking::GetInstance()->X_MOVE_AMPLITUDE = x_move;	//Straight speed
		Robot::Walking::GetInstance()->A_MOVE_AMPLITUDE = a_move;	//Turn speed
	}
	else {
		int pre_amp = Robot::Walking::GetInstance()->X_MOVE_AMPLITUDE;
		int iterval = msec / abs(pre_amp - x_move);
		#ifndef NDEBUG
			printf("Slowly: Previous: %d, Current: %d, Iterval: %d", pre_amp, x_move, iterval);
		#endif
		for (int i = pre_amp; i != x_move;) {
			Robot::Walking::GetInstance()->X_MOVE_AMPLITUDE = i;
			usleep(iterval * 1000);
			if (pre_amp > x_move) {
				--i;
			}
			else {
				++i;
			}
		}
	}
	
}

bool Motion::fall_up()
{
	using namespace Robot;
	#ifndef NDEBUG
		printf("Status::FALLEN: %d\n", MotionStatus::FALLEN);
	#endif

	if(MotionStatus::FALLEN != STANDUP)
	{
		Walking::GetInstance()->Stop();
		while(Walking::GetInstance()->IsRunning() == 1)
			usleep(8000);

		Action::GetInstance()->m_Joint.SetEnableBody(true, true);

		if(MotionStatus::FALLEN == FORWARD)
			Action::GetInstance()->Start(10);   //Forwrad getup
		else if(MotionStatus::FALLEN == BACKWARD)
			Action::GetInstance()->Start(11);   //Backward getup

		while(Action::GetInstance()->IsRunning() == 1) usleep(8000);

		Head::GetInstance()->m_Joint.SetEnableHeadOnly(true, true);
		Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true, true);
		return true;
	}
	return false;
}

void Motion::head_move(int x, int y, bool home)
{
	#ifndef NDEBUG
		printf("Head MoveByAngle: %d, %d\n", x, y);
	#endif
	if (home)
		Robot::Head::GetInstance()->MoveToHome();
	Robot::Head::GetInstance()->MoveByAngle(x, y);
}

void Motion::action(int index)
{
	Robot::Action::GetInstance()->Start(index);
	while (Robot::Action::GetInstance()->IsRunning())
		usleep(8 * 1000);
}
