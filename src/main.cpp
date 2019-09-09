//this is the test driver
#include "main.h"
//*********************************************//
//*********************************************//
//main driver function
int main()
{
	//initialize Earth
	Earth.init();

	//load the map file
	Earth.load_map("world.map");

	//initialize the simulation to initial conditions
	Earth.ic();

	//main loop
	while (true)
	{
		//main compute loop
		Earth.compute(1.0);

		//check for key break
		if (GetAsyncKeyState(VK_ESCAPE) != 0)
			break;		
		//DEBUG
		if (Earth.reference_time > 10000.0)
			break;
	}

	//unload the map file 
	Earth.unload_map();

	//perform exit procedures
	Earth.exit();

	getchar();

	//done.
	return 0;
}

