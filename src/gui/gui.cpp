#include "gui.h"



int main(int argc, char* argv[])
{
	GUIController* gc;
	gc = new GUIController(320,240);
	gc->start(argc, argv);


	delete gc;
	/* code */
	return 0;
}