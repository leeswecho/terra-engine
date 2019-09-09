#include "cPopulation.h"
#include "cKnowledge.h"
#include "cTerrain.h"

//for random
#include <random>

class cNation
{
public:

	//the start index of this nation
	int start_index;

	//These are the current attributes of the nation.
	//they are updated in an a continuing loop as the characteristics of the terrain change.

	//the current prestiges of each of the different job occupations
	double Prestiges[NUM_LABOR_IDS];
	double total_prestige; //keep track of total prestige
	//the current yields of each job occupation. Note that the "units", or "meaning" of the numbers
	//(i.e. calories/acre-day, research/day, etc.) are specific to the job described.
	//these numbers are primarily measures of how efficient the nation currently is at performing these tasks
	double Yields[NUM_LABOR_IDS];
	//the current "reference" usages of resources (land, facilities, etc.) needed to produce yields.
	//(i.e. acres/person, office-space/person, etc.)...units are again specific to the job described.
	double Intensities[NUM_LABOR_IDS];

	//the current discoveries by discovery index, held by this nation
	bool Discoveries[NUM_DISCOVERIES];
	//the number of discoveries in each field, held by this nation
	int Fields[NUM_FIELDS];

	//ages of labor: age at which the poplation is able to participate in a task
	int AgeLaborLow;
	int AgeLaborHigh;
	//ages of fertility: age at which the population is able to have children
	int AgeFertilityLow;
	int AgeFertilityHigh;

	//simulation init to initial conditions
	int ic();
	//run-time compute function
	int compute();
	//minor compute function
	int compute_minor(double time_step);

	//utility functions to calculate various quantities

	//these two functions are at national level because they require
	//knowledge of various national ages
	int calc_num_households(cTerrain* pTerrain);
	int calc_num_children(cTerrain* pTerrain);
	int calc_num_laborers(cTerrain* pTerrain);
	//updates labor array based on new data
	int update_division_of_labor(cTerrain* pTerrain, double num_laborers, double num_new_laborers);
	//compute food production (i.e., calories)
	int update_food_production(cTerrain* pTerrain);
	//update total prestige
	int update_total_prestige();
	//compute the fertility rate
	fertilitycurve calc_fertility(double children_per_household);
	//update the health rating of an area
	int update_health(cTerrain* pTerrain);
	//utility function to return the rate of cancer
	double get_cancer_rate(int age);
	//utility function to get the starvation rate
	double get_starvation_rate(int age, cTerrain* pTerrain);
	//utility function to "integerize" a random probability
	int random_event(double sample, double rate);
	//function to update death rates
	int update_death_rates(cTerrain* pTerrain);
	//update the birthdate
	int update_birth_rate(cTerrain* pTerrain, int num_households, double c0, double c1, double c2);
	

	//initialization function
	int init();
	//exit function
	int exit();

private:


};