#include "Agent.h"
#include "Parameters.h"
#include "FileHandling.h"
#include <random>
#include <cmath>
#include <ctime>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>


Agent::Agent(FileHandling *file): _file(file), ID(0), key(0), ageCat(0), age(0), race(0), gender(-1), income(0), borough(0), yrsEmployed(0), subInc(0), 
	indIncome(0), avgHHIncome(0), avgHHSaving(0), hhSize(0), damage(0), lossElectric(NO_ELECTRIC_LOSS), lossHeat(NO_HEAT_LOSS), lossWater(NO_WATER_LOSS), /*lossFinancial(NO_FINANCIAL_LOSS),*/ 
	lossFinancialType(0), lossFinancialAmnt(0), leftHome(-1), PTSDstatus(-1), PTSDx(0) 
{

}

Agent::~Agent()
{
	//std::cout << "Agent class destructor called! " << std::endl;
}


void Agent::setAgentAttributes(int idx)
{
	ID = idx;

	ageRaceGenderBoroDist();
	householdSizeDist();
	houseIncomeDist();
	houseSubIncomeDist();

	key = 100*borough + 10*income + hhSize;

	yrsEmployed = randomIntegerGenerator(1, age - 16);
	yrsEmployed = (yrsEmployed > 5) ? randomIntegerGenerator(1, 5) : yrsEmployed;
}

//@proportions obtained by cross classification of age, race and gender estimates from ACS 2008-2012 by census tract of affected areas
void Agent::ageRaceGenderBoroDist()
{
	double randomP = uniformRealDist();
	for(unsigned int i = 0; i < _file->_newYork.size(); i++)
	{
		if(randomP < _file->_newYork.at(i).pDemoAtt)
		{
			borough = _file->_newYork.at(i).inBorough;
			ageCat = _file->_newYork.at(i).inAge;
			race = _file->_newYork.at(i).inRace;
			gender = _file->_newYork.at(i).inGender;
			break;
		}
	}

	//assigns age to agents based on their age category
	if(ageCat == AGE1){age = randomIntegerGenerator(AGE1_MIN, AGE1_MAX);}
	else if(ageCat == AGE2){age = randomIntegerGenerator(AGE2_MIN, AGE2_MAX);}
	else if(ageCat == AGE3){age = randomIntegerGenerator(AGE3_MIN, AGE3_MAX);}
}

void Agent::householdSizeDist()
{
	/*double randomP = uniformRealDist();
	for(unsigned int i = 0; i < _file->_newYorkHHsize.size(); i++)
	{
		if(borough == _file->_newYorkHHsize.at(i).inBoro)
		{
			if(randomP < _file->_newYorkHHsize.at(i).pHHsize)
			{
				hhSize = _file->_newYorkHHsize.at(i).inHHSize;
				break;
			}
		}
	}*/

	double randomP = uniformRealDist();
	auto its = _file->_newYorkHHmap.equal_range(borough);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP < it->second.pHHsize)
		{
			hhSize = it->second.inHHSize;
			break;
		}
	}
}

//@proportions used from ACS estimates 2008-2012 for household income

void Agent::houseIncomeDist()
{
	/*double randomP = uniformRealDist();
	for(unsigned int i = 0; i < _file->_newYorkIncome.size(); i++)
	{
		if(hhSize == _file->_newYorkIncome.at(i).inHHsize && borough == _file->_newYorkIncome.at(i).inBoro)
		{
			if(randomP < _file->_newYorkIncome.at(i).pIncome)
			{
				income = _file->_newYorkIncome.at(i).inIncome;
				break;
			}
		}
	}*/

	double randomP = uniformRealDist();
	int incKey = 10*hhSize + borough;
	auto its = _file->_newYorkIncMap.equal_range(incKey);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP < it->second.pIncome)
		{
			income = it->second.inIncome;
			break;
		}
	}
	
}

void Agent::houseSubIncomeDist()
{
	/*double randomP = uniformRealDist();
	for(unsigned int i = 0; i < _file->_newYorkIncDiv.size(); i++)
	{
		if(income == _file->_newYorkIncDiv.at(i).inIncome && borough == _file->_newYorkIncDiv.at(i).inBoro)
		{
			if(randomP < _file->_newYorkIncDiv.at(i).pSubInc)
			{
				subInc = _file->_newYorkIncDiv.at(i).inSubIncome;
				indIncome = randomIntegerGenerator(_file->_newYorkIncDiv.at(i).minIncome, _file->_newYorkIncDiv.at(i).maxIncome);
				break;
			}
		}
	}*/

	double randomP = uniformRealDist();
	int subIncKey = 10*income + borough;
	auto its = _file->_newYorkSubIncMap.equal_range(subIncKey);

	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP < it->second.pSubInc)
		{
			subInc = it->second.inSubIncome;
			indIncome = randomIntegerGenerator(it->second.minIncome, it->second.maxIncome);
			break;
		}
	}
}

void Agent::decayPTSDsymptom()
{
	
	PTSDx = PTSDx - 0.015*uniformRealDist()*PTSDx;
	
}


int Agent::randomIntegerGenerator(int min, int max)
{
	/*std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution <int> dist(min, max);

	int randVal = dist(generator);
	return randVal;*/

	static boost::mt19937 rng(time(NULL));

	boost::random::uniform_int_distribution<>dist(min, max);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<>>generator(rng, dist);

	int randVal = generator();

	return randVal;
}

double Agent::uniformRealDist()
{
	/*std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_real_distribution <double> dist(0, 1);

	double randomP = dist(generator);

	return randomP;*/

	//return ((double)rand()/RAND_MAX);

	static boost::mt19937 rng(time(NULL)); http://stackoverflow.com/questions/2254909/boost-random-number-generator

	boost::random::uniform_real_distribution<>dist(0, 1);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_real_distribution<>>generator(rng, dist);

	double randVal = generator();

	return randVal;

}


int Agent::getID() const 
{
	return ID;
}

int Agent::getKey() const
{
	return key;
}

int Agent::getAgeCat() const 
{ 
	return ageCat;
}

int Agent::getAge() const
{
	return age;
}

int Agent::getRace() const
{ 
	return race;
}

int Agent::getGender() const
{ 
	return gender;
}

int Agent::getYearEmployed() const
{
	return yrsEmployed;
}

int Agent::getIncome() const
{ 
	return income;
}

int Agent::getSubIncome() const
{ 
	return subInc;
}

int Agent::getIndvIncome()const
{ 
	return indIncome;
}

float Agent::getAvgHHIncome() const
{
	return avgHHIncome;
}

float Agent::getAvgHHSaving() const
{
	return avgHHSaving;
}

int Agent::getBorough() const
{ 
	return borough;
}

int Agent::getHHsize() const
{ 
	return hhSize;
}
int Agent::getDamage() const
{
	return damage;
}

int Agent::getElectricLoss() const 
{ 
	return lossElectric;
}

int Agent::getHeatLoss() const 
{ 
	return lossHeat;
}

int Agent::getWaterLoss() const 
{ 
	return lossWater;
}

//int Agent::getFinancialLoss() const
//{
//	return lossFinancial;
//}

int Agent::getFinancialLossType() const
{
	return lossFinancialType;
}

int Agent::getFinancialLossAmnt() const
{
	return lossFinancialAmnt;
}


int Agent::getLeftHome() const
{
	return leftHome;
}

int Agent::getPTSDstatus() const
{
	return PTSDstatus;
}

double Agent::getPTSDsymptom() const
{
	return PTSDx;
}

void Agent::setAvgHHIncome(float avgFamilyIncome)
{
	this->avgHHIncome = avgFamilyIncome;
}

void Agent::setAvgHHSaving(float avgFamilySaving)
{
	this->avgHHSaving = avgFamilySaving;
}

void Agent::setDamage(int houseDamage)
{
	this->damage = houseDamage;
}

void Agent::setElectricLoss(int electricLoss)
{
	this->lossElectric = electricLoss;
}

void Agent::setHeatLoss(int heatLoss)
{
	this->lossHeat = heatLoss;
}

void Agent::setWaterLoss(int waterLoss)
{
	this->lossWater = waterLoss;
}

//void Agent::setFinancialLoss(int financialLoss)
//{
//	this->lossFinancial = financialLoss;
//}

void Agent::setFinancialLossType(int financialLossType)
{
	this->lossFinancialType = financialLossType;
}

void Agent::setFinancialLossAmnt(int financialLossAmnt)
{
	this->lossFinancialAmnt = financialLossAmnt;
}

void Agent::setLeftHome(int leftHouse)
{
	this->leftHome = leftHouse;
}

void Agent::setPTSDstatus(int statusPTSD)
{
	this->PTSDstatus = statusPTSD;
}

void Agent::setPTSDsymptom(double inPTSDx)
{
	this->PTSDx = inPTSDx;
}




