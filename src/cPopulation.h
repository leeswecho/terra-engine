#pragma once

#include <random>
//defines the maximum age of people(after which point they automatically die)
#define MAXIMUM_AGE  120
//the number of healthy dailty calories per person
#define DAILY_CALORIES 2000

//these are labors, things that people can do
#define NUM_LABOR_IDS		100
#define LABOR_ID_HUNTER		1
#define LABOR_ID_GATHERER	2
#define LABOR_ID_FARMER		3

//this class represents a single "atomic" group of people.
class cPopulation
{

public:
	//the number of people in each age, in this population
	int Cohorts[MAXIMUM_AGE];
	//the number of working-age people working each job
	double Labors[NUM_LABOR_IDS];
	//the nationality (i.e. nation index) of the inhabitants
	int nationality; 

	//internal compute function for population
	int compute();
	//minor compute function. (applies birth and death rates)
	int compute_minor(double time_step);

	//function to rescale the labor array
	int rescale_labors(double new_total);

	//utiity functions to calculate the number of ... things
	int calc_num_people();

	//advance the population by one year
	int advance_age(); 

	//utility function
	int random_event(double sample, double rate);

	//initializer
	int init();
	//finalizer
	int exit();



	//some house-keeping processing variables
	int num_households;
	int num_children;
	int num_people;
	int num_laborers;
	//the health of the population
	double health;
	//the total number of calories produced a day
	double calories;
	//the number of calories per day per person
	double cppd;
	//birth and death rates (per year)
	double birth_rate;
	double death_rates_cancer[MAXIMUM_AGE];
	double death_rates_starvation[MAXIMUM_AGE];

private:

};

//this is a structure that encapsulates a fertility curve
class fertilitycurve
{
public:
	double c0; 
	double c1;
	double c2;
};