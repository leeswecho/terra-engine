#include "cNation.h"
//************************************************//
//************************************************//
//initialization function
int cNation::init()
{
	//set to invalid value
	start_index = -1;

	//done.
	return 0;
}
//************************************************//
//************************************************//
//utility function to compute number of households in area
int cNation::calc_num_households(cTerrain* pTerrain)
{
	//return variable
	int ret = 0;

	//loop over age range
	for (int i = AgeFertilityLow; i < AgeFertilityHigh; i++)
	{
		ret += pTerrain->AttachedPopulation.Cohorts[i];
	}

	//return result
	return ret;
}
//************************************************//
//************************************************//
//utility function to compute number of laborers in area
int cNation::calc_num_laborers(cTerrain* pTerrain)
{
	//return variable
	int ret = 0;

	//loop over age range
	for (int i = AgeLaborLow; i < AgeLaborHigh; i++)
	{
		ret += pTerrain->AttachedPopulation.Cohorts[i];
	}

	//return result
	return ret;
}
//************************************************//
//************************************************//
//utility function to compute number of children in area
int cNation::calc_num_children(cTerrain* pTerrain)
{
	//return variable
	int ret = 0;

	//loop over age range
	for (int i = 0; i < AgeFertilityLow; i++)
	{
		ret += pTerrain->AttachedPopulation.Cohorts[i];
	}

	//return result
	return ret;

}
//************************************************//
//************************************************//
//utility function to compute the fertility rate
fertilitycurve cNation::calc_fertility(double children_per_household)
{
	//create a return value
	fertilitycurve ret;

	//set the three constants based on cph:
	//this is the chance, in a given year, that you can/will choose to have a child
    //if you have not had a child in the previous year
	ret.c0 = 0.33 / (1.0 + children_per_household);
	//c1 = this is the same chance, except that you did have a child in the last year
	ret.c1 = 0.15 / (1.0 + children_per_household);
	//c2 = two years ago
	ret.c2 = 0.2 / (1.0 + children_per_household);

	//return the result
	return ret;

}
//************************************************//
//************************************************//
int cNation::update_division_of_labor(cTerrain* pTerrain, double num_laborers, double num_new_laborers)
{
	//get the labor array
	double* Labors = pTerrain->AttachedPopulation.Labors;
	//get the current health value
	double health = pTerrain->AttachedPopulation.health;

	//this has two components: the first is the component that is currently in the workforce
	double old_laborers = num_laborers - num_new_laborers;

	//don't do any of the above if old laborers is 0
	if (old_laborers > 0)
	{
        //adjust the importance of food gathering duties (hunting, farming, gathering)
        //by the health ratio
		Labors[LABOR_ID_FARMER] = Labors[LABOR_ID_FARMER] / health;
		Labors[LABOR_ID_HUNTER] = Labors[LABOR_ID_HUNTER] / health;
		Labors[LABOR_ID_GATHERER] = Labors[LABOR_ID_GATHERER] / health;
		//rescale the list
		pTerrain->AttachedPopulation.rescale_labors(old_laborers);
		//now do rescaling for farmers
		if (Labors[LABOR_ID_FARMER] > 0)
		{
			//now adjust the number of laborers against the available resources if everybody got what they wanted
			double acres_farmland_per_farmer = (pTerrain->Composition[TERRAIN_TYPE_PLAINS] - pTerrain->Overlays[OVERLAY_CITIES]) / Labors[LABOR_ID_FARMER];
			//get the "labor" ratio (square of acres projected over acres optimal)
            //I'm not sure if the resulting adjustment curve will make sense but let's give it a try
			double farming_ratio = acres_farmland_per_farmer / Intensities[LABOR_ID_FARMER];
			//then square the farming ratio
			farming_ratio *= farming_ratio;
			//adjust the labor by this ratio
			Labors[LABOR_ID_FARMER] *= farming_ratio;
		}
		//do for the other two
		double acres_forest_per_hunter = pTerrain->Composition[TERRAIN_TYPE_FOREST] / Labors[LABOR_ID_HUNTER];
		double acres_forest_per_gatherer = pTerrain->Composition[TERRAIN_TYPE_FOREST] / Labors[LABOR_ID_GATHERER];
		double hunting_ratio = acres_forest_per_hunter / Intensities[LABOR_ID_HUNTER];
		hunting_ratio *= hunting_ratio; //square it
		double gathering_ratio = acres_forest_per_gatherer / Intensities[LABOR_ID_GATHERER];
		gathering_ratio *= gathering_ratio; //square it
		//scalre by ratio
		Labors[LABOR_ID_HUNTER] *= hunting_ratio;
		Labors[LABOR_ID_GATHERER] *= gathering_ratio;
		
		//okay, now rescale the labors again
		pTerrain->AttachedPopulation.rescale_labors(old_laborers);
	}

	//now, add the effect of new laborers
	//new laborers start out with their preferences directly
	//(i.e. new laborers basically start out with what they want to do, directly)
	Labors[LABOR_ID_FARMER] += num_new_laborers * (Prestiges[LABOR_ID_FARMER] / total_prestige);
	Labors[LABOR_ID_HUNTER] += num_new_laborers * (Prestiges[LABOR_ID_HUNTER] / total_prestige);
	Labors[LABOR_ID_GATHERER] += num_new_laborers * (Prestiges[LABOR_ID_GATHERER] / total_prestige);

	//done.
	return 0;

}
//************************************************//
//************************************************//
//calculate food production in a given tile
//note that this value is production rate per day
int cNation::update_food_production(cTerrain* pTerrain)
{
	//get the labor array
	double* Labors = pTerrain->AttachedPopulation.Labors;
	//get the current health value
	double health = pTerrain->AttachedPopulation.health;

	//start with 0
	double calories = 0;

	//compute production from farming
	if (Labors[LABOR_ID_FARMER] > 0)
	{
		double acres_farmland_per_farmer = (pTerrain->Composition[TERRAIN_TYPE_PLAINS] - pTerrain->Overlays[OVERLAY_CITIES]) / Labors[LABOR_ID_FARMER];
		//cap at acres farmable
		if (acres_farmland_per_farmer > Intensities[LABOR_ID_FARMER])
			acres_farmland_per_farmer = Intensities[LABOR_ID_FARMER];
		//determine yield
		calories += Labors[LABOR_ID_FARMER] * health * acres_farmland_per_farmer * Yields[LABOR_ID_FARMER];
	}
	//do the same for hunting
	if (Labors[LABOR_ID_HUNTER] > 0)
	{
		double acres_forest_per_hunter = pTerrain->Composition[TERRAIN_TYPE_FOREST] / Labors[LABOR_ID_HUNTER];
		if (acres_forest_per_hunter > Intensities[LABOR_ID_HUNTER])
			acres_forest_per_hunter = Intensities[LABOR_ID_HUNTER];
		calories += Labors[LABOR_ID_HUNTER] * health * acres_forest_per_hunter * Yields[LABOR_ID_HUNTER];
	}
	//do the same for gathering
	if (Labors[LABOR_ID_GATHERER] > 0)
	{
		double acres_forest_per_gatherer = pTerrain->Composition[TERRAIN_TYPE_FOREST] / Labors[LABOR_ID_GATHERER];
		if (acres_forest_per_gatherer > Intensities[LABOR_ID_GATHERER])
			acres_forest_per_gatherer = Intensities[LABOR_ID_GATHERER];
		calories += Labors[LABOR_ID_GATHERER] * health * acres_forest_per_gatherer * Yields[LABOR_ID_GATHERER];
	}

	//return calories produced
	pTerrain->AttachedPopulation.calories = calories;

	//done.
	return 0;
		
}
//************************************************//
//************************************************//
//update area health
int cNation::update_health(cTerrain* pTerrain)
{
	//update the area cppd
	pTerrain->AttachedPopulation.cppd = pTerrain->AttachedPopulation.calories / pTerrain->AttachedPopulation.num_people;

	//if there are enough calories, we're done, just return 1.0
	if (pTerrain->AttachedPopulation.cppd > DAILY_CALORIES)
		return 1.0;

	//otherwise, take 1 minute the square of the percentage deficiency
	double defc = 1.0 - (pTerrain->AttachedPopulation.cppd/DAILY_CALORIES);

	//if we get over 1, limit to 0.99
	if (defc > 0.99)
		defc = 0.99;

	//update health
	pTerrain->AttachedPopulation.health = 1.0 - (defc*defc);

	//done.
	return 0;
}
//************************************************//
//************************************************//
double cNation::get_cancer_rate(int age)
{
	//rate increases linearly until age 40,
    //at which it is 100.0 cases per 100,000 (0.001)
	if (age < 40)
		return 0.001*(age/40.0);
	//otherwise, it increases exponentially
	double x = age - 40;
	return 0.001 + 0.0002*x*x;
}
//************************************************//
//************************************************//
//get starvation rate
double cNation::get_starvation_rate(int age, cTerrain* pTerrain)
{
	//get the cppd
	double cppd = pTerrain->AttachedPopulation.cppd;

	//
	double defc = 1.0 - (cppd / DAILY_CALORIES);

	//if we're above, return no starvation
	if (defc <= 0.0)
		return 0;

	//otherwise return the cube of the deficiency
	return defc*defc*defc;

}
//************************************************//
//************************************************//
int cNation::update_death_rates(cTerrain* pTerrain)
{
	//update the cancer and starvation rates for all ages
	for (int i = 0; i < MAXIMUM_AGE; i++)
	{
		pTerrain->AttachedPopulation.death_rates_cancer[i] = get_cancer_rate(i);
		pTerrain->AttachedPopulation.death_rates_starvation[i] = get_starvation_rate(i, pTerrain);
	}

	return 0;
}
//************************************************//
//************************************************//
int cNation::update_birth_rate(cTerrain* pTerrain, int num_households, double c0, double c1, double c2)
{
	//start accumulating the birth rate
	double birth_rate = 0;

	//get the attached population
	cPopulation* cP = &pTerrain->AttachedPopulation;

    //compute the number of households who have not had children either last year or year before last.
    //note that this is an estimate that miscalculates two possibilities:
    //1) households that had children both last year and year before last
    //2) households that had children in the previous years but have died since then
	int num_c0_households = num_households - cP->Cohorts[0] - cP->Cohorts[1];

	//account for the possibly that the number could be less than 0 due to the above factors, and correct
	if (num_c0_households < 0)
		num_c0_households = 0;

	//now compute the number of children born to c0 households
	birth_rate += num_c0_households * c0;

	//add the c1 rate
	birth_rate += cP->Cohorts[0] * c1;

	//compute the number of c2 households (this is less than cohorts[1] due to households that had children both years)
	double num_c2_households = cP->Cohorts[1] - cP->Cohorts[0] * c1;  //the second term is an estimate

	//account for case it could be 0
	if (num_c2_households < 0.0)
		num_c2_households = 0;

	//now add the c2 contribution
	birth_rate += num_c2_households * c2;

	//set the final birth rate
	cP->birth_rate = birth_rate;

	//done.
	return 0;
}
//************************************************//
//************************************************//
int cNation::compute_minor(double time_step)
{

	//done.
	return 0;

}
//************************************************//
//************************************************//
int cNation::random_event(double sample, double rate)
{
	//get the normal
	double ret = sample*rate;
	//now truncate
	int i_ret = int(ret);
	//get the remainder
	double rem = ret - i_ret;
	//save the old
	ret = i_ret;
	//now get a random number
	double rnd = double(rand()) / RAND_MAX;

	//now check if we're above the threshold
	if (rem >= rnd)
		ret += 1;
	
	//return the integerized value
	return int(ret);
}
//************************************************//
//************************************************//
//update total prestige
int cNation::update_total_prestige()
{
	//reset
	total_prestige = 0;

	//loop through
	for (int i = 0; i < NUM_LABOR_IDS; i++)
		total_prestige += Prestiges[i];

	//done.
	return 0;
}
//************************************************//
//************************************************//
//set inital conditions
int cNation::ic()
{
	//set initial prestiges
	Prestiges[LABOR_ID_FARMER] = 2.0;
	Prestiges[LABOR_ID_HUNTER] = 1.0;
	Prestiges[LABOR_ID_GATHERER] = 1.0;

	//

	//done.
	return 0;
}
//************************************************//
//************************************************//
//run-time compute function
int cNation::compute()
{
	//TEMP right now hard-code various national attributes internally 

	//set all general yields, prestiges and intensities to 0
	for (int i = 0; i < NUM_LABOR_IDS; i++)
	{
		Yields[i] = 0;
		Intensities[i] = 0;
		Prestiges[i] = 0.0;
	}

	//now hard-code some yield values:
	//Farmer/Hunter/Gatherer (calories per acre per day)
	Yields[LABOR_ID_FARMER] = 1250.0;
	Yields[LABOR_ID_HUNTER] = 100.0;
	Yields[LABOR_ID_GATHERER] = 200.0;

	//hard-code some prestige values
	Prestiges[LABOR_ID_FARMER] = 2.0;
	Prestiges[LABOR_ID_HUNTER] = 1.0;
	Prestiges[LABOR_ID_GATHERER] = 1.0;

	//hard-code soeme labor intensity values
	//Farmer/Hunter/Gatherer: acres per person
	Intensities[LABOR_ID_FARMER] = 4.0;
	Intensities[LABOR_ID_HUNTER] = 60.0;
	Intensities[LABOR_ID_GATHERER] = 30.0;

	//hard-code age of labor
	AgeLaborLow = 18;
	AgeLaborHigh = 50;

	//hard-code age of fertility
	AgeFertilityLow = 18;
	AgeFertilityHigh = 42;

	//recalculate total prestige
	update_total_prestige();

	//done.
	return 0;
}
//************************************************//
//************************************************//
//exit function
int cNation::exit()
{



	//done.
	return 0;
}
//************************************************//
//************************************************//