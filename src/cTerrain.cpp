#include "cTerrain.h"
//***************************************************//
//***************************************************//
int cTerrain::init()
{
	
	//set all terrain numbers to 0
	for (int i = 0; i < NUM_TERRAIN_TYPES; i++)
		Composition[i] = 0;

	//set all overlay numbers to 0
	for (int j = 0; j < NUM_OVERLAY_TYPES; j++)
		Overlays[j] = 0;
	
	//initialize all neighbors to 0
	for (int k = 0; k < 8; k++)
		Neighbors[k] = -1;

	//set terrain validity to false
	bEnabled = false;
	
	//set location to invalid value
	fLongitude = 0.0;
	fLatitude = 0.0;
	frLong = 0.0;
	frLat = 0.0;

	//reset neighbors
	iNumNeighbors = 0;

	//call init on attached population
	AttachedPopulation.init();

	//done.
	return 0;
}
//***************************************************//
//***************************************************//
int cTerrain::exit()
{
	
	//call exit on attached population
	AttachedPopulation.exit();

	//done.
	return 0;
}
//***************************************************//
//***************************************************//
//compute function for terrain
int cTerrain::compute()
{

	//done.
	return 0;
}
//***************************************************//
//***************************************************//