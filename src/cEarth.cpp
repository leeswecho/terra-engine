#include "cEarth.h"
//*****************************************************************//
//*****************************************************************//
//initialize
int cEarth::init()
{
	//reset variables
	iNumNations = 0;
	iNumTerrain = 0;
	reference_time = 0.0;
	last_major_compute = -1.0;
	//
	Nations = NULL;
	Terrain = NULL;
	
	//map not loaded yet
	bMapLoaded = false;

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//utility function to process the NTIL command
int cEarth::parse_map_ntil(char* buffer)
{
	//there's only one parameter so just atoi the rest of the line
	int num_tiles = atoi(buffer);

	//reject invalid values
	if (num_tiles <= 0)
		return -1;

	//allocate pointers for the map and set the size
	iNumTerrain = num_tiles;
	Terrain = new cTerrain[num_tiles];

	//call init function for all tiles
	for (int i = 0; i < num_tiles; i++)
	{
		Terrain[i].init();
	}

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//utility function to process the TLEN command
int cEarth::parse_map_tlen(char* buffer)
{
	//there's only one parameter so just atoi the rest of the line
	int index = atoi(buffer);

	//reject invalid values
	if (index <= 0)
		return -1;

	//otherwise, set enable
	Terrain[index].bEnabled = true;

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//utility function to process the NNTN command
int cEarth::parse_map_nntn(char* buffer)
{

	//there's only one parameter so just atoi the rest of the line
	int num_nations = atoi(buffer);

	//reject invalid values
	if (num_nations <= 0)
		return -1;

	//allocate pointers for the map and set the size
	iNumNations = num_nations;
	Nations = new cNation[iNumNations];

	//call init function for all tiles
	for (int i = 0; i < num_nations; i++)
	{
		Nations[i].init();
	}
	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
int cEarth::parse_map_ntsl(char* buffer)
{
	//save the position of the pipe
	int separator = -1;

	//search for a pipe to separate the line
	for (int i = 0; i < strlen(buffer); i++)
	{
		//if we find it,
		if (buffer[i] == '|')
		{
			//make it a zero, set the position, and break
			buffer[i] = 0;
			separator = i;
			break;
		}
	}
	//if we failed to find a pipe, return with error
	if (separator == -1)
		return -1;

	//otherwise get the index first
	int nation = atoi(buffer);

	//check for error
	if (nation < 0)
		return -1;
	if (nation >= iNumNations)
		return -1;

	//now try to get the  nation number 
	int index = atoi(buffer+separator+1);
	
	//check for error
	if (index < 0)
		return -1;
	if (index >= iNumTerrain)
		return -1;

	//then set it
	Nations[nation].start_index = index;

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//utility function to parse TILF command 
int cEarth::parse_map_tlxx(int terrain_type, char* buffer)
{
	//save the position of the pipe
	int separator = -1;

	//search for a pipe to separate the line
	for (int i = 0; i < strlen(buffer); i++)
	{
		//if we find it,
		if (buffer[i] == '|')
		{
			//make it a zero, set the position, and break
			buffer[i] = 0;
			separator = i;
			break;
		}
	}
	//if we failed to find a pipe, return with error
	if (separator == -1)
		return -1;

	//otherwise get the index first
	int index = atoi(buffer);

	//check for error
	if (index < 0)
		return -1;
	if (index >= iNumTerrain)
		return -1;

	//now try to get the amount 
	int area = atoi(buffer+separator+1);
	
	//check for error
	if (area <= 0)
		return -1;

	//then set it
	Terrain[index].Composition[terrain_type] = area;

	//done.
	return 0;

}
//*****************************************************************//
//*****************************************************************//
int cEarth::parse_map_tl_location(int location_type, char* buffer)
{

	//save the position of the pipe
	int separator = -1;

	//search for a pipe to separate the line
	for (int i = 0; i < strlen(buffer); i++)
	{
		//if we find it,
		if (buffer[i] == '|')
		{
			//make it a zero, set the position, and break
			buffer[i] = 0;
			separator = i;
			break;
		}
	}
	//if we failed to find a pipe, return with error
	if (separator == -1)
		return -1;

	//otherwise get the index first
	int index = atoi(buffer);

	//check for error
	if (index < 0)
		return -1;
	if (index >= iNumTerrain)
		return -1;

	//now try to get the amount 
	double location = atof(buffer+separator+1);
	
	//check for error
	if (location <= 0)
		return -1;

	//otherwise, set the area
	if (location_type == 1)
		Terrain[index].fLongitude = location;
	if (location_type == 2)
		Terrain[index].fLatitude = location;
	if (location_type == 11)
		Terrain[index].frLong = location;
	if (location_type == 12)
		Terrain[index].frLat = location;

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//do IC for each nation
int cEarth::ic_nation(int nationality)
{
	//retrieve the start location for the nation
	int start_index = Nations[nationality].start_index;

	//initialize the population at this start index
	Terrain[start_index].AttachedPopulation.nationality = nationality;
	//initialize the population at 10,000...TEMP for now do this by putting 250
	//people at each age betwween 0 and 40
	for (int i = 0; i < 40; i++)
		Terrain[start_index].AttachedPopulation.Cohorts[i] = 250;

	for (int k = 0; k < NUM_LABOR_IDS; k++)
		Nations[nationality].Prestiges[k] = 0.0;

	//set default values
	Nations[nationality].Prestiges[LABOR_ID_FARMER] = 2.0;
	Nations[nationality].Prestiges[LABOR_ID_HUNTER] = 1.0;
	Nations[nationality].Prestiges[LABOR_ID_GATHERER] = 1.0;

	//set labors as equal
	for (int j = 0; j < NUM_LABOR_IDS; j++)
		Terrain[start_index].AttachedPopulation.Labors[j] = Nations[nationality].Prestiges[j];

	//set initial health at 1.0
	Terrain[start_index].AttachedPopulation.health = 1.0;

	//run internal IC
	Nations[nationality].ic();

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//function to initialize simulation
int cEarth::ic()
{
	//call IC on all nations
	for (int i = 0; i < iNumNations; i++)
	{
		ic_nation(i);
	}

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//load map file
int cEarth::load_map(char* map_name)
{
	//a line buffer
	char line[300]; //lot of padding here

	//open the file
	FILE* pFile = fopen(map_name, "r");

	//read out the first line
	fgets(line, 300, pFile);
	//this first line must be "TERRA" otherwise  it will be rejected
	//as an invalid file
	if (strlen(line) >= 5)
	{
		//strip last character (which is the return character)
		line[strlen(line)-1] = 0;
		if (strcmp(line, "TERRA") != 0)
		{
			int a = strcmp(line, "TERRA");

			return -1; //invalid file
		}
	}

	//otherwise read out the file line by line
	while ( fgets(line, 300, pFile) != NULL)
	{
		//strip last character (which is the return character)
		line[strlen(line)-1] = 0;

		//return value
		int ret = 0;
		
		//if line is longer than 5
		if (strlen(line) >= 5)
		if (line[0] != '#')  //and first line is not a comment
		{
			//create a four character buffer
			char cc4[5];
			//copy the first four characters to this buffer
			memset(cc4, 0, 5);
			memcpy(cc4, line, 4);

			//now perform actions based on this buffer:
			//NTIL: sets the number of tiles
			if (strcmp(cc4, "NTIL") == 0)
				ret = parse_map_ntil(line + 5);
			//NNTN: sets the number of nations
			else if (strcmp(cc4, "NNTN") == 0)
				ret = parse_map_nntn(line + 5);
			//NTSL: sets the start index of the nation
			else if (strcmp(cc4, "NTSL") == 0)
				ret = parse_map_ntsl(line + 5);
			//TLEN: enable a tile
			else if (strcmp(cc4, "TLEN") == 0)
				ret = parse_map_tlen(line + 5);
			//TLFR: sets the amount of forest in the tile in acres
			else if (strcmp(cc4, "TLFR") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_FOREST, line + 5);
			//TLOC: sets the amount of ocean in the tile in acres
			else if (strcmp(cc4, "TLOC") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_OCEAN, line + 5);
			//TLOC: sets the amount of mountain in the tile in acres
			else if (strcmp(cc4, "TLMT") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_MOUNTAIN, line + 5);
			else if (strcmp(cc4, "TLGR") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_GRASSLAND, line + 5);
			else if (strcmp(cc4, "TLDS") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_DESERT, line + 5);
			else if (strcmp(cc4, "TLWT") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_WATER, line + 5);
			else if (strcmp(cc4, "TLPL") == 0)
				ret = parse_map_tlxx(TERRAIN_TYPE_PLAINS, line + 5);
			else if (strcmp(cc4, "TLON") == 0)
				ret = parse_map_tl_location(1, line + 5);
			else if (strcmp(cc4, "TLAT") == 0)
				ret = parse_map_tl_location(2, line + 5);
			else if (strcmp(cc4, "TDLO") == 0)
				ret = parse_map_tl_location(11, line + 5);
			else if (strcmp(cc4, "TDLT") == 0)
				ret = parse_map_tl_location(12, line + 5);

			//if any of the commands failed, break and quit
			if (ret < 0)
			{
				bMapLoaded = false;
				fclose(pFile);
				return -1;
			}
		}
	}
	
	//otherwise set enabled to true
	bMapLoaded = true;

	//close the file
	fclose(pFile);

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//unload map file
int cEarth::unload_map()
{
	//if map is not loaded, quit
	if (bMapLoaded == false)
		return -1;

	//call exit on all terrain
	for (int i = 0; i < iNumTerrain; i++)
		Terrain[i].exit();

	//otherwise de-allocate
	delete [] Terrain;
	Terrain = NULL;

	//call exit on all nations
	for (int j = 0; j < iNumNations; j++)
		Nations[j].exit();

	//de-allocate
	delete [] Nations;
	Nations = NULL;

	//set map loaded to false
	bMapLoaded = false;

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//exit
int cEarth::exit()
{

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//compute for a single terrain tile
int cEarth::compute_tile(cTerrain* pTerrain)
{

	//DEBUG
	printf("Time: %f Health: %f Population: %d\n", reference_time, pTerrain->AttachedPopulation.health, pTerrain->AttachedPopulation.num_people);


	//reject invalid input
	if (pTerrain == NULL)
		return -1;

	//get the governing nation index
	int nation = pTerrain->AttachedPopulation.nationality;

	//compute various figures:
	//first, the total population
	pTerrain->AttachedPopulation.num_people = pTerrain->AttachedPopulation.calc_num_people();
	//then, the number of laborers
	pTerrain->AttachedPopulation.num_laborers = Nations[nation].calc_num_laborers(pTerrain);
	//get the number of new laborers
	int AgeLaborLow = Nations[nation].AgeLaborLow;
	int num_new_laborers = pTerrain->AttachedPopulation.Cohorts[AgeLaborLow];

	//determine the division of labor
	Nations[nation].update_division_of_labor(pTerrain, pTerrain->AttachedPopulation.num_laborers, num_new_laborers);

	//update the food production
	Nations[nation].update_food_production(pTerrain);

	//update health
	Nations[nation].update_health(pTerrain);

	//update death rates
	Nations[nation].update_death_rates(pTerrain);

	//compute more figures relative to fertility:
	//compute number of households
	pTerrain->AttachedPopulation.num_households = Nations[nation].calc_num_households(pTerrain);
	pTerrain->AttachedPopulation.num_children = Nations[nation].calc_num_children(pTerrain);

	//compute number of children per household
	double cph = 0.0;
	if (pTerrain->AttachedPopulation.num_households != 0.0)
		cph = pTerrain->AttachedPopulation.num_children / pTerrain->AttachedPopulation.num_households;

	//compute the fertility curve
	fertilitycurve fc = Nations[nation].calc_fertility(cph);

	//use the fertility curve to compute birth rates
	Nations[nation].update_birth_rate(pTerrain, pTerrain->AttachedPopulation.num_households, fc.c0, fc.c1, fc.c2);
	
	//advance the age of the population by one year
	pTerrain->AttachedPopulation.advance_age();

	//run internal compute
	pTerrain->compute();

	//run internal compute for attached population
	pTerrain->AttachedPopulation.compute();


	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//compute step for nation
int cEarth::compute_nation(int nationality)
{
	//update national attributes 
	//(based on new developments, discoveries, etc.)

	//run internal compute
	Nations[nationality].compute();

	///done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//
//main compute step
int cEarth::compute(double time_step)
{
	//increment reference time
	reference_time += time_step;

	//if a year has passed since the last compute
	if ((reference_time - last_major_compute) >= 1.0)
	{	
		//go over all the nations and run their compute fuunctions
		for (int i = 0; i < iNumNations; i++)
		{
			compute_nation(i);
		}

		//go over all of the terrain tiles and run their compute functions
		for (int j = 0; j < iNumTerrain; j++)
		{
			//run compute function
			compute_tile(&Terrain[j]);
		}

		//update compute time
		last_major_compute = reference_time;
	}

	//run minor compute events
	//go over all the nations and run their compute fuunctions
	for (int k = 0; k < iNumNations; k++)
	{
		Nations[k].compute_minor(time_step);
	}

	//go over all of the terrain tiles and run their compute functions
	for (int m = 0; m < iNumTerrain; m++)
	{
		//run compute function
		Terrain[m].AttachedPopulation.compute_minor(time_step);
	}

	//done.
	return 0;
}
//*****************************************************************//
//*****************************************************************//

