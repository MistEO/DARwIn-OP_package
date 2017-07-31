#include "detector.h"

int main(int argc, char ** argv)
{
	Detector::ins().insert("test");
	Detector::ins().adjust_color("test");
	while (true) {
		Detector::ins().refresh();
		Detector::ins().process("test");
		Detector::ins().show();
	}
	return 0;
}
