#include "Family.h"
#include "Agent.h"
#include "FileHandling.h"
#include <random>
#include <ctime>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

Family::Family(int index) : familyID(index), familyKey(0), houseDamage(0), electricLoss(NO_ELECTRIC_LOSS), heatLoss(NO_HEAT_LOSS), waterLoss(NO_WATER_LOSS), /*financialLoss(NO_FINANCIAL_LOSS),*/ financialLossType(0),
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
	_agentFamList.push_back(_agent);
}

void Family::calcAverageFamilyIncome()
{
	for(auto agent = _agentFamList.begin(); agent != _agentFamList.end(); ++agent)
	{
		avgFamilyIncome += agent->getIndvIncome();
		avgEmploymentDuration += agent->getYearEmployed();
	}

	avgFamilyIncome = avgFamilyIncome/_agentFamList.size();
	avgEmploymentDuration = avgEmploymentDuration / _agentFamList.size(); 
}

void Family::calcAverageFamilySaving(FileHandling *file)
{
	/*for(unsigned int i = 0; i < file->_newYorkExpense.size(); i++)
	{
		if(familySize == file->_newYorkExpense.at(i).inHHsize)
		{
			if(avgFamilyIncome >= file->_newYorkExpense.at(i).minIncome && avgFamilyIncome < file->_newYorkExpense.at(i).maxIncome)
			{
				avgIncomeSaved = avgEmploymentDuration*avgFamilyIncome*(1-file->_newYorkExpense.at(i).ratioExpenseInc);
				break;
			}
		}
	}*/

	auto its = file->_newYorkExpenseMap.equal_range(familySize);
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
	//FileHandling *file = new FileHandling();
	//file->readSandyDamageFile();

	/*double randomP = uniformRealDist();
	for(unsigned int i = 0; i < file->_newYorkSandyDamage.size(); i++)
	{
		if(familyIncome == file->_newYorkSandyDamage.at(i).inIncome && familyBorough == file->_newYorkSandyDamage.at(i).inBoro)
		{
			if(randomP < file->_newYorkSandyDamage.at(i).pSandyDamage)
			{
				houseDamage = file->_newYorkSandyDamage.at(i).inDamage;
				break;
			}
		}
	}*/

	double randomP = uniformRealDist();
	int damageKey = 10*familyIncome + familyBorough;
	auto its = file->_newYorkSandyDamageMap.equal_range(damageKey);
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

	for(auto agent = _agentFamList.begin(); agent != _agentFamList.end(); ++agent)
	{
		double PTSDx = 0;
		int incDecline = 0;

		agent->setDamage(houseDamage);
		agent->setElectricLoss(electricLoss);
		agent->setHeatLoss(heatLoss);
		agent->setWaterLoss(waterLoss);
		//agent->setFinancialLoss(financialLoss);
		agent->setFinancialLossType(financialLossType);
		agent->setFinancialLossAmnt(financialLossAmnt);
		agent->setAvgHHIncome(avgFamilyIncome);
		agent->setAvgHHSaving(avgIncomeSaved);

		agent->setLeftHome(predictEvacuation(*agent));
		agent->setPTSDstatus(predictPTSDstatus(file, agent->getLeftHome(), PTSDx, incDecline));
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
	double logElectricLoss = -0.4290206 + (-0.6557606*bronx) + (-0.3706641*brooklyn) + (.3009517*queens) + (2.319021*staten) + (1.712744*modDamage) + (2.521294*severeDamage);
	double pElectricLoss = exp(logElectricLoss) / (1 + exp(logElectricLoss));

	double randomP = uniformRealDist();
	if(randomP < pElectricLoss){electricLoss = ELECTRIC_LOSS;}
}

void Family::predictHeatLoss()
{
	double logHeatLoss = -2.700497 + (-0.892628*bronx) + (-0.5164573*brooklyn) + (-0.4032009*queens) + (-0.6797956*staten) + (1.491232*modDamage) + (2.017177*severeDamage) + (3.381449*electricLoss);
	double pHeatLoss = exp(logHeatLoss) / (1 + exp(logHeatLoss));

	double randomP = uniformRealDist();

	if(randomP < pHeatLoss){heatLoss = HEAT_LOSS;}
}

void Family::predictWaterLoss()
{
	double logWaterLoss = -3.784657 + (-2.495215*bronx) + (-0.3513104*brooklyn) + (-1.219524*queens) + (-2.044849*staten) + (0.2851809*modDamage) + (1.128114*severeDamage) + (2.070515*electricLoss) 
		+ (1.685371*heatLoss);
	double pWaterLoss = exp(logWaterLoss) / (1 + exp(logWaterLoss));

	double randomP = uniformRealDist();

	if(randomP < pWaterLoss){waterLoss = WATER_LOSS;}
}

void Family::predictEconomicLoss(FileHandling *file)
{
	//subclassification of economic loss into 3 categories

	//if(houseDamage == MODERATE_DAMAGE)
	//{
	//	double randomP = uniformRealDist();
	//	for(unsigned int i = 0; i < file->_newYorkSandyFinLoss.size(); i++)
	//	{
	//		if(familyBorough == file->_newYorkSandyFinLoss.at(i).inBorough)
	//		{
	//			if(randomP < file->_newYorkSandyFinLoss.at(i).pFinLossType)
	//			{
	//				financialLossType = file->_newYorkSandyFinLoss.at(i).inFinLossType; //sets financial loss to type1(1-10K) or type2(10K-25k)
	//				financialLossAmnt = randomFinancialLossGenerator(file->_newYorkSandyFinLoss.at(i).minLossAmnt, file->_newYorkSandyFinLoss.at(i).maxLossAmnt);
	//				break;
	//			}
	//		}
	//	}
	if(houseDamage == MODERATE_DAMAGE)
	{
		double randomP = uniformRealDist();
		auto its = file->_newYorkSandyFinLossMap.equal_range(familyBorough);
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

	//avgFamilyIncome = avgFamilyIncome - (float)financialLossAmnt;
	//if(avgFamilyIncome < 0.0f) {avgFamilyIncome = 0.f;}

	////update the income household income category after the financial loss
	//if(avgFamilyIncome >= INCOME1_MIN && avgFamilyIncome < INCOME2_MIN) {familyIncome = INCOME1;}
	//else if(avgFamilyIncome >= INCOME2_MIN && avgFamilyIncome < INCOME3_MIN) {familyIncome = INCOME2;}
	//else if(avgFamilyIncome >= INCOME3_MIN && avgFamilyIncome < INCOME3_MAX) {familyIncome = INCOME3;}

}

int Family::predictEvacuation(Agent a)
{
	//dummy variables related to age
	int age1 = -1; 
	int age2 = -1;
	int age3 = -1;

	int leftHome = -1;

	if(a.getAgeCat() == AGE1){age1 = 1; age2 = 0; age3 = 0;}
	else if(a.getAgeCat() == AGE2){age1 = 0; age2 = 1; age3 = 0;}
	else if(a.getAgeCat() == AGE3){age1 = 0; age2 = 0; age3 = 1;}

	double logEvacuation = -3.120275 + (-0.658522*age2) + (-1.162498*age3) + (1.097261*modDamage) + (2.880528*severeDamage) + (1.600197*electricLoss) + (1.014325*heatLoss) + (1.101851*waterLoss);
	double pEvacuation = exp(logEvacuation) / (1 + exp(logEvacuation));

	if(uniformRealDist() < pEvacuation){leftHome = LEFT_HOME_YES;}
	else {leftHome = LEFT_HOME_NO;}

	return leftHome;
}

int Family::predictPTSDstatus(FileHandling *file, int leftHome, double &PTSDx, int &incDecline)
{
	int PTSDstatus = -1;
	//int incomeLoss = (financialLossType == FINANCIAL_LOSS_TYPE1 || financialLossType == FINANCIAL_LOSS_TYPE2 || financialLossType == FINANCIAL_LOSS_TYPE3) ? 1 : 0;
	float incomeLoss = avgIncomeSaved - (float)financialLossAmnt;
	float propIncomeLoss = (incomeLoss > 0) ? (float)financialLossAmnt/avgIncomeSaved : 0;

	incDecline = ((incomeLoss < 0 && financialLossAmnt != 0) || (incomeLoss > 0 && propIncomeLoss > 0.5)) ? 2 : 1;
	
	/*double randomP = uniformRealDist();
	for(unsigned int i = 0; i < file->_newYorkSandyPTSD.size(); i++)
	{
		if(leftHome == file->_newYorkSandyPTSD.at(i).inLeftHome && incDecline == file->_newYorkSandyPTSD.at(i).inIncomeLoss)
		{
			if(randomP < file->_newYorkSandyPTSD.at(i).pSandyPTSD)
			{
				PTSDstatus = file->_newYorkSandyPTSD.at(i).inPTSDstatus;
				PTSDx = file->_newYorkSandyPTSD.at(i).inPTSDx;
				break;
			}
		}
	}*/
	double randomP = uniformRealDist();
	auto its = file->_newYorkSandyPTSDMap.equal_range(10*incDecline+leftHome);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP < it->second.pSandyPTSD)
		{
			PTSDstatus = it->second.inPTSDstatus;
			PTSDx = it->second.inPTSDx;
			break;
		}
	}

	return PTSDstatus;
}



int Family::getFamilyID() const
{
	return familyID;
}

int Family::getFamilyKey() const
{
	return familyKey;
}

int Family::getFamilyRace() const
{
	return familyRace;
}

int Family::getFamilyIncome() const
{
	return familyIncome;
}


int Family::getFamilyBorough() const
{
	return familyBorough;
}

int Family::getFamilySize() const
{
	return familySize;
}

int Family::getHouseDamage() const
{
	return houseDamage;
}

int Family::getFinancialLossType() const
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

double Family::uniformRealDist()
{
	/*std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_real_distribution <double> dist(0, 1);

	double randomP = dist(generator);

	return randomP;*/

	//return ((double)rand()/RAND_MAX);

	static boost::mt19937 rng(time(NULL));

	boost::random::uniform_real_distribution<>dist(0, 1);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_real_distribution<>>generator(rng, dist);

	double randVal = generator();

	return randVal;

}

int Family::randomFinancialLossGenerator(int min, int max)
{
	/*std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution <int> dist(min, max);

	int randFinLoss = dist(generator);
	return randFinLoss;*/

	static boost::mt19937 rng(time(NULL));

	boost::random::uniform_int_distribution<>dist(min, max);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<>>generator(rng, dist);

	int randVal = generator();

	return randVal;

}

