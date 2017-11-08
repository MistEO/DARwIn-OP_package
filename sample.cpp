#include "detector.h"
#include <iostream>

int main(int argc, char ** argv)
{
	Detector::ins().insert("test", false, 2);
	Detector::ins().insert("test2", true);
	Detector::ins().adjust_color("test");
	Detector::ins().adjust_color("test2");
	while (true) {
		Detector::ins().refresh();
		Detector::ins().process("test");
		Detector::ins().process("test2");
		std::cout << Detector::ins().get_rect_attr("test", 2, Area) << std::endl;
		Detector::ins().show();
	}
	return 0;
}
