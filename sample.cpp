#include "detector.h"

int main(int argc, char ** argv)
{
	Detector::ins().insert("test", true);
	Detector::ins().adjust_color("test");
	Detector::ins().insert("test2");
	Detector::ins().adjust_color("test2");
	while (true) {
		Detector::ins().refresh();
		Detector::ins().process("test");
		Detector::ins().process("test2");
		Detector::ins().show();
	}
	return 0;
}
