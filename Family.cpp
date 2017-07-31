#include "Family.h"
#include "Agent.h"
#include "FileHandling.h"
#include <random>
#include <ctime>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

Family::Family(unsigned int index) : familyID(index), familyKey(0), houseDamage(0), electricLoss(NO_ELECTRIC_LOSS), heatLoss(NO_HEAT_LOSS), waterLoss(NO_WATER_LOSS), financialLossType(0),
	financialLossAmnt(0)
{
	bronx = -1;
	brooklyn = -1;
	manhattan = -1;
	queens = -1;
	staten = -1;

	noDamage = -1;
	modDamage = -1;
	severeDamage = -1;
}

Family::~Family()
{
}


void Family::setFamilyAttributes(Agent _agent)
{
	familyKey = _agent.getKey();
	familyRace = _agent.getRace();
	familyIncome = _agent.getIncome();
	familyBorough = _agent.getBorough();
	familySize = _agent.getHHsize();
	avgFamilyIncome = 0;
	avgEmploymentDuration = 0;
	avgIncomeSaved = 0;
}

void Family::addAgent(Agent _agent)
{
	agentFamList.push_back(_agent);
}

void Family::calcAverageFamilyIncome()
{
	for(auto agent = agentFamList.begin(); agent != agentFamList.end(); ++agent)
	{
		avgFamilyIncome += agent->getIndvIncome();
		avgEmploymentDuration += agent->getYearEmployed();
	}

	avgFamilyIncome = avgFamilyIncome/agentFamList.size();
	avgEmploymentDuration = avgEmploymentDuration / agentFamList.size(); 
}

void Family::calcAverageFamilySaving(FileHandling *file)
{
	auto its = file->newYorkExpenseMap.equal_range(familySize);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(avgFamilyIncome >= it->second.minIncome && avgFamilyIncome < it->second.maxIncome)
		{
			avgIncomeSaved = avgEmploymentDuration*avgFamilyIncome*(1-it->second.ratioExpenseInc);
			break;
		}
	}
}

//@uses proportions from Sandy data to predict the damage caused to home by Hurricane Sandy based on income and borough
void Family::predictSandyDamage(FileHandling *file)
{
	float randomP = uniformRealDist();
	int damageKey = 10*familyIncome + familyBorough;
	auto its = file->newYorkSandyDamageMap.equal_range(damageKey);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP < it->second.pSandyDamage)
		{
			houseDamage = it->second.inDamage;
			break;
		}
	}

	
	setDummyVariables();
	
	predictElectricLoss();
	predictHeatLoss();
	predictWaterLoss();
	predictEconomicLoss(file);

	for(auto agent = agentFamList.begin(); agent != agentFamList.end(); ++agent)
	{
		float PTSDx = 0;
		unsigned short int incDecline = 0;

		agent->setDamage(houseDamage);
		agent->setElectricLoss(electricLoss);
		agent->setHeatLoss(heatLoss);
		agent->setWaterLoss(waterLoss);
		//agent->setFinancialLoss(financialLoss);
		agent->setFinancialLossType(financialLossType);
		agent->setFinancialLossAmnt(financialLossAmnt);
		agent->setAvgHHIncome(avgFamilyIncome);
		agent->setAvgHHSaving(avgIncomeSaved);

		agent->setLeftHome(predictEvacuation(agent->getAgeCat()));
		agent->setPTSDstatus(predictPTSDstatus(file, agent->getAgeCat(), agent->getGender(), agent->getLeftHome(), PTSDx, incDecline));
		agent->setPTSDsymptom(PTSDx);
		agent->setIncomeDecline(incDecline);

	}
}

void Family::setDummyVariables()
{
	//set dummy variables for boroughs
	if(getFamilyBorough() == BRONX){bronx = 1; brooklyn = 0; manhattan = 0; queens = 0; staten = 0;}
	else if(getFamilyBorough() == BROOKLYN){bronx = 0; brooklyn = 1; manhattan = 0; queens = 0; staten = 0;}
	else if(getFamilyBorough() == MANHATTAN){bronx = 0; brooklyn = 0; manhattan = 1; queens = 0; staten = 0;}
	else if(getFamilyBorough() == QUEENS){bronx = 0; brooklyn = 0; manhattan = 0; queens = 1; staten = 0;}
	else if(getFamilyBorough() == STATEN){bronx = 0; brooklyn = 0; manhattan = 0; queens = 0; staten = 1;}

	//set dummy variables for damage
	if(getHouseDamage() == NO_DAMAGE) {noDamage = 1; modDamage = 0; severeDamage = 0;}
	else if(getHouseDamage() == MODERATE_DAMAGE) {noDamage = 0; modDamage = 1; severeDamage = 0;}
	else if(getHouseDamage() == SEVERE_DAMAGE) {noDamage = 0; modDamage = 0; severeDamage = 1;}
}

void Family::predictElectricLoss()
{
	float logElectricLoss = -0.4290206 + (-0.6557606*bronx) + (-0.3706641*brooklyn) + (.3009517*queens) + (2.319021*staten) + (1.712744*modDamage) + (2.521294*severeDamage);
	float pElectricLoss = exp(logElectricLoss) / (1 + exp(logElectricLoss));

	float randomP = uniformRealDist();
	if(randomP < pElectricLoss){electricLoss = ELECTRIC_LOSS;}
}

void Family::predictHeatLoss()
{
	float logHeatLoss = -2.700497 + (-0.892628*bronx) + (-0.5164573*brooklyn) + (-0.4032009*queens) + (-0.6797956*staten) + (1.491232*modDamage) + (2.017177*severeDamage) + (3.381449*electricLoss);
	float pHeatLoss = exp(logHeatLoss) / (1 + exp(logHeatLoss));

	float randomP = uniformRealDist();

	if(randomP < pHeatLoss){heatLoss = HEAT_LOSS;}
}

void Family::predictWaterLoss()
{
	float logWaterLoss = -3.784657 + (-2.495215*bronx) + (-0.3513104*brooklyn) + (-1.219524*queens) + (-2.044849*staten) + (0.2851809*modDamage) + (1.128114*severeDamage) + (2.070515*electricLoss) 
		+ (1.685371*heatLoss);
	float pWaterLoss = exp(logWaterLoss) / (1 + exp(logWaterLoss));

	float randomP = uniformRealDist();

	if(randomP < pWaterLoss){waterLoss = WATER_LOSS;}
}

void Family::predictEconomicLoss(FileHandling *file)
{
	if(houseDamage == MODERATE_DAMAGE)
	{
		float randomP = uniformRealDist();
		auto its = file->newYorkSandyFinLossMap.equal_range(familyBorough);
		for(auto it = its.first; it != its.second; ++it)
		{
			if(randomP < it->second.pFinLossType)
			{
				financialLossType = it->second.inFinLossType;
				financialLossAmnt = randomFinancialLossGenerator(it->second.minLossAmnt, it->second.maxLossAmnt);
				break;
			}
		}
	}
	else if(houseDamage == SEVERE_DAMAGE)
	{
		financialLossType = FINANCIAL_LOSS_TYPE3; 
		financialLossAmnt = randomFinancialLossGenerator(MIN_LOSS_TYPE3, MAX_LOSS_TYPE3);
	}
}

unsigned short int Family::predictEvacuation(unsigned short int ageCat)
{
	//dummy variables related to age
	short int age1 = -1; 
	short int age2 = -1;
	short int age3 = -1;

	short int leftHome = -1;

	if(ageCat == AGE1){age1 = 1; age2 = 0; age3 = 0;}
	else if(ageCat == AGE2){age1 = 0; age2 = 1; age3 = 0;}
	else if(ageCat == AGE3){age1 = 0; age2 = 0; age3 = 1;}

	float logEvacuation = -3.120275 + (-0.658522*age2) + (-1.162498*age3) + (1.097261*modDamage) + (2.880528*severeDamage) + (1.600197*electricLoss) + (1.014325*heatLoss) + (1.101851*waterLoss);
	float pEvacuation = exp(logEvacuation) / (1 + exp(logEvacuation));

	float randomP = uniformRealDist();
	if(randomP < pEvacuation){leftHome = LEFT_HOME_YES;}
	else {leftHome = LEFT_HOME_NO;}

	return leftHome;
}

unsigned short int Family::predictPTSDstatus(FileHandling *file, unsigned short int ageCat, unsigned short int gender, unsigned short int leftHome, float &PTSDx, unsigned short int &incDecline)
{
	short int PTSDstatus = -1;
	//int incomeLoss = (financialLossType == FINANCIAL_LOSS_TYPE1 || financialLossType == FINANCIAL_LOSS_TYPE2 || financialLossType == FINANCIAL_LOSS_TYPE3) ? 1 : 0;
	float incomeLoss = avgIncomeSaved - (float)financialLossAmnt;
	float propIncomeLoss = (incomeLoss > 0) ? (float)financialLossAmnt/avgIncomeSaved : 0;

	incDecline = ((incomeLoss < 0 && financialLossAmnt != 0) || (incomeLoss > 0 && propIncomeLoss > 0.5)) ? INCOME_LOSS_YES : INCOME_LOSS_NO;
	
	float randomP = uniformRealDist();
	//auto its = file->newYorkSandyPTSDMap.equal_range(10*incDecline+leftHome);
	auto its = file->newYorkSandyPTSDMap.equal_range(100*incDecline+10*gender+ageCat);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP < it->second.pSandyPTSD)
		{
			PTSDstatus = it->second.inPTSDstatus;
			//PTSDx = it->second.inPTSDx;
			break;
		}
	}

	//assign PTSD symptom to agents 
	//int keyPTSDx = 10000*ageCat+1000*gender+100*leftHome+10*incDecline+PTSDstatus;
	int keyPTSDx = 1000*incDecline+100*leftHome+10*ageCat+PTSDstatus;
	auto its1 = file->newYorkSandyPTSDsympMap.equal_range(keyPTSDx);

	for(auto it1 = its1.first; it1 != its1.second; ++it1)
	{
		PTSDx = (it1->second > PTSD_CUT_OFF) ? (it1->second-3) : it1->second;
		break;
	}

	return PTSDstatus;
}


unsigned int Family::getFamilyID() const
{
	return familyID;
}

unsigned short int Family::getFamilyKey() const
{
	return familyKey;
}

unsigned short int Family::getFamilyRace() const
{
	return familyRace;
}

int Family::getFamilyIncome() const
{
	return familyIncome;
}


unsigned short int Family::getFamilyBorough() const
{
	return familyBorough;
}

unsigned short int Family::getFamilySize() const
{
	return familySize;
}

unsigned short int Family::getHouseDamage() const
{
	return houseDamage;
}

unsigned short int Family::getFinancialLossType() const
{
	return financialLossType;
}

int Family::getFinancialLossAmnt() const
{
	return financialLossAmnt;
}

float Family::getAvgFamilyIncome() const
{
	return avgFamilyIncome;
}

float Family::getAvgFamilySaving() const
{
	return avgIncomeSaved;
}

float Family::getAvgEmploymentDuration() const
{
	return avgEmploymentDuration;
}

float Family::uniformRealDist()
{

	static boost::mt19937 rng(time(NULL));

	boost::random::uniform_real_distribution<>dist(0, 1);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_real_distribution<>>generator(rng, dist);

	float randVal = generator();

	return randVal;

}

int Family::randomFinancialLossGenerator(int min, int max)
{

	static boost::mt19937 rng(time(NULL));

	boost::random::uniform_int_distribution<>dist(min, max);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<>>generator(rng, dist);

	int randVal = generator();

	return randVal;

}

