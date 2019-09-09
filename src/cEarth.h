#include "cNation.h"
#include "cTerrain.h"
#include "cPopulation.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NULL 0

//This class represents the entire planet, and holds access to the entire simulation
class cEarth
{
public:

	//this stores the "master" time of the system
	double reference_time;
	//save the last time a "major" compute was done.
	double last_major_compute;

	//initialization of system
	int init();
	//this loads a map file
	int load_map(char* map_name);
	//this unloads a map file
	int unload_map();

	//shut down of system
	int exit();

	//function to initialize the simulation
	int ic();
	//Earth-level IC functions that need to be done for each nation
	int ic_nation(int nationality);

	//main compute step of system
	int compute(double time_step);
	//sub-ordinate functions that perform the busy work:
	//processes a single terrain tile
	int compute_tile(cTerrain* pTerrain);
	//processes tasks for a nation
	int compute_nation(int nationality);

private:
	//utility function to parse each section
	int parse_map_ntil(char* buffer);
	int parse_map_nntn(char* buffer);
	int parse_map_ntsl(char* buffer);
	int parse_map_tlxx(int terrain_type, char* buffer);
	int parse_map_tl_location(int location_type, char* buffer);
	int parse_map_tlen(char* buffer);

	//the array of terrain
	cTerrain* Terrain;
	//the array of nations
	cNation* Nations;

	//the number of terrain tiles
	int iNumTerrain;
	int iNumNations;
	//a parameter to keep track of whether the map was successfully loaded
	bool bMapLoaded;


};