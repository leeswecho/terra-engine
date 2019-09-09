#pragma once

//import the cPopulation definition
#include "cPopulation.h"

//defines for terrain types
#define NUM_TERRAIN_TYPES 10
#define TERRAIN_TYPE_GRASSLAND  1
#define TERRAIN_TYPE_OCEAN		2	
#define TERRAIN_TYPE_WATER		3 //i.e. freshwater
#define TERRAIN_TYPE_FOREST		4
#define TERRAIN_TYPE_DESERT		5
#define TERRAIN_TYPE_MOUNTAIN	6
#define TERRAIN_TYPE_PLAINS		7

#define NUM_OVERLAY_TYPES	10
#define OVERLAY_FARMS		1
#define OVERLAY_CITIES		2

//this represents a single map tile
class cTerrain
{
public:
	//composition
	double Composition[NUM_TERRAIN_TYPES];
	//overlays
	double Overlays[NUM_OVERLAY_TYPES];
	//location  (center)
	double fLongitude;
	double fLatitude;
	//"width" and "height"
	double frLong;
	double frLat;
	//an array of neighbor indexes
	int Neighbors[8];
	int iNumNeighbors;
	//the attached poualation of this class
	cPopulation AttachedPopulation;

	//whether or not this tile is enabled
	bool bEnabled;


	//compute function for terrain
	int compute();

	//init function for the terrain
	int init();
	//exit function for the terrain
	int exit();


private:

};