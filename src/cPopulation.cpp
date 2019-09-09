#include "cPopulation.h"
//*********************************************************//
//*********************************************************//
int cPopulation::init()
{
	//initialize all cohort values to 0
	for (int i = 0; i < MAXIMUM_AGE; i++)
		Cohorts[i] = 0;

	//initialize all labor values to 0
	for (int j = 0; j < NUM_LABOR_IDS; j++)
		Labors[j] = 0;
	
	//initialize nationality to -1 (i.e. invalid)
	nationality = -1;

	//done.
	return 0;
}
//*********************************************************//
//*********************************************************//
int cPopulation::exit()
{

	//done.
	return 0;
}
//*********************************************************//
//*********************************************************//
//utility function to compute total number of people
int cPopulation::calc_num_people()
{
	//return variable
	int ret = 0;

	//loop through
	for (int i = 0; i < MAXIMUM_AGE; i++)
		ret += Cohorts[i];

	return ret;
}
//*********************************************************//
//*********************************************************//
//utility function to rescale the labor array so that it scales up to the supplied total
int cPopulation::rescale_labors(double new_total)
{
	//get the current total
	double curr_total = 0.0;
	for (int i = 0; i < NUM_LABOR_IDS; i++)
		curr_total += Labors[i];

	//now go through again and scale by the total
	for (int j = 0; j < NUM_LABOR_IDS; j++)
		Labors[j] *= new_total / curr_total;

	//done.
	return 0;
}
//*********************************************************//
//*********************************************************//
//compute function
int cPopulation::compute()
{

	//done.
	return 0;
}
//*********************************************************//
//*********************************************************//
int cPopulation::compute_minor(double time_step)
{
	//calculate births during this time step
	//the birth rate is already scaled to the total population
	//so there's no need to set the sample
	int births = random_event(1.0, birth_rate);
	//add that to year 0
	Cohorts[0] += births;

	//compute deaths for each age
	for (int i = 0; i < MAXIMUM_AGE; i++)
	{
		//
		int num_cancer_deaths = random_event(Cohorts[i], death_rates_cancer[i]);
		//subtract from group
		Cohorts[i] -= num_cancer_deaths;
		//
		int num_starvation_deaths = random_event(Cohorts[i], death_rates_starvation[i]);
		//subtract from group
		Cohorts[i] -= num_starvation_deaths;
	}

	//done.
	return 0;
}
//*********************************************************//
//*********************************************************//
int cPopulation::advance_age()
{
	//advance the population by one year
	for (int i = (MAXIMUM_AGE-1); i > 0; i--)
		Cohorts[i] = Cohorts[i-1];

	//now finally reset cohorts[0]
	Cohorts[0] = 0;

	//done.
	return 0;
}
//*********************************************************//
//*********************************************************//
//handle random events, but quantized
int cPopulation::random_event(double sample, double rate)
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
//*********************************************************//
//*********************************************************//
