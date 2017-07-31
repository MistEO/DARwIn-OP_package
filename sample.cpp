#include "motion.h"
#include "detector.h"
int main(int argc, char ** argv)
{
	Detector::ins().insert("test");
	Detector::ins().adjust_color("test");
	return 0;
}
