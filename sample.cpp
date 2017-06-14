#include "motion.h"
#include "objectdetector.h"
#include <iostream>
#include <signal.h>

void got_signal(int);

int main(int argc, char ** argv)
{
    signal(SIGABRT, got_signal);
    signal(SIGTERM, got_signal);
    signal(SIGQUIT, got_signal);
    signal(SIGINT, got_signal);

    const int x_speed = 30;
    const int speed_up_msec = 200;

	ObjectDetector obj("test", ShowBinary | ShowSource | ShowDrawing);
	obj.adjust_color();

    getchar();
    Motion::ins().walk_start();
    Motion::ins().walk(x_speed, 0, speed_up_msec);
    while (true) {
		obj.process_by_color(10);
		std::cout << "Postion: " << obj.postion() << " Width: " << obj.width() << std::endl;
        if (Motion::ins().fall_up()) {
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
