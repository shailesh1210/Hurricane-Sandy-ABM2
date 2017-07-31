#include "Agent.h"
#include "Parameters.h"
#include "FileHandling.h"
#include "skeleton.h"


Agent::Agent(Skeleton *world, FileHandling *file): _world(world), _file(file), ID(0), key(0), ageCat(0), age(0), race(0), gender(-1), income(0), borough(0), yrsEmployed(0), subInc(0), 
	indIncome(0), avgHHIncome(0), avgHHSaving(0), hhSize(0), damage(0), lossElectric(NO_ELECTRIC_LOSS), lossHeat(NO_HEAT_LOSS), lossWater(NO_WATER_LOSS), lossFinancialType(0), 
	lossFinancialAmnt(0), incDeclineT0(0), leftHome(-1), leftHomeT0(-1),initPTSDx(0),  weeksDisplaced(0), priorCBT(0), priorSPR(0), cbtReferred(false), sprReferred(false), financialHelpType(0)
{
	for(unsigned int i = 0; i < NUM_TREATMENT; i++)
	{
		incDecline[i] = 0;
		PTSDstatus[i] = -1;
		curPTSDx[i] = 0;
		state[i] = EXCELLENT;
		currCBT[i] = false;
		currSPR[i] = false;
		sessionsCBT[i] = 0;
		sessionsSPR[i] = 0;
		ptsdResolved[i] = false;
		reimburseTime[i] = 0;
	}
}

Agent::~Agent()
{
	//std::cout << "Agent class destructor called! " << std::endl;
}


void Agent::setAgentAttributes(unsigned int idx)
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

void Agent::executeAgentRules(unsigned short int currTime)
{
	predictPriorTrtmentEnrollment(currTime);
	countWeeksDisplaced(currTime);
	ptsdScreeningSteppedCare(currTime);

	for(unsigned short int i = STEPPED_CARE_WITH_SS; i <= USUAL_CARE_WITHOUT_SS; i++)
	{
		updateAgentState(i);
		ptsdTreatment(currTime, i);
		ptsdSymptomResolution(currTime, i);
		financialReimbursement(currTime, i);
	}

	//steppedCareWithSocialWork(currTime);
	//steppedCareWithoutSocialWork(currTime);
	//usualCareWithoutSocialWork(currTime);
	//relapsePTSDsymptom(currTime);
}

//@proportions obtained by cross classification of age, race and gender estimates from ACS 2008-2012 by census tract of affected areas
void Agent::ageRaceGenderBoroDist()
{
	float randomP1 = uniformRealDist();
	for(unsigned int i = 0; i < _file->newYorkBoro.size(); i++)
	{
		if(randomP1 < _file->newYorkBoro.at(i).pBorough)
		{
			borough = _file->newYorkBoro.at(i).inBorough;
			break;
		}
	}

	float randomP2 = uniformRealDist();
	auto its = _file->newYorkMap.equal_range(borough);
	for(auto it = its.first; it != its.second; ++it)
	{
		if(randomP2 < it->second.pDemoAtt)
		{
			ageCat = it->second.inAge;
			race = it->second.inRace;
			gender = it->second.inGender;
			break;
		}
	}

	//assigns age to agents based on their age category
	if(ageCat == AGE1){age = randomIntegerGenerator(AGE1_MIN, AGE1_MAX);}
	else if(ageCat == AGE2){age = randomIntegerGenerator(AGE2_MIN, AGE2_MAX);}
	else if(ageCat == AGE3){age = randomIntegerGenerator(AGE3_MIN, AGE3_MAX);}

	//initalize dummy variables

	if(ageCat == AGE1){age1 = 1; age2 = 0; age3 = 0;}
	else if(ageCat == AGE2){age1 = 0; age2 = 1; age3 = 0;}
	else if(ageCat == AGE3){age1 = 0; age2 = 0; age3 = 1;}
	
	if(race == WHITE){white = 1; black = 0; hispanic = 0; otherRace = 0;}
	else if(race == BLACK){white = 0; black = 1; hispanic = 0; otherRace = 0;}
	else if(race == HISP){white = 0; black = 0; hispanic = 1; otherRace = 0;}
	else if(race == OTHER){white = 0; black = 0; hispanic = 0; otherRace = 1;}

}

void Agent::householdSizeDist()
{
	float randomP = uniformRealDist();
	auto its = _file->newYorkHHmap.equal_range(borough);
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
	float randomP = uniformRealDist();
	int incKey = 10*hhSize + borough;
	auto its = _file->newYorkIncMap.equal_range(incKey);
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
	float randomP = uniformRealDist();
	int subIncKey = 10*income + borough;
	auto its = _file->newYorkSubIncMap.equal_range(subIncKey);

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


void Agent::predictPriorTrtmentEnrollment(unsigned short int currTime)
{
	//dummy variables related to age, gender and race;
	if(currTime == 0)
	{
		unsigned short int dGender = (gender == MALE) ? 1 : 0;

		float logCBT = 0; 
		float logSPR = 0;

		if(initPTSDx <= _world->ptsdxCutoff)
		//if(initPTSDx <= PTSD_CUT_OFF)
		{
			logCBT = -1.3360 + (-0.0881*dGender) + (-0.7250*black) + (-0.3133*hispanic) + (-0.7122*otherRace) + (0.6201*age2) + (-0.6444*age3);
			logSPR = -1.095 + (-0.2119*dGender) + (-0.5343*black) + (-0.2377*hispanic) + (-0.7551*otherRace) + (0.5693*age2) + (-0.5413*age3);
		}
		else if(initPTSDx > _world->ptsdxCutoff)
		//else if(initPTSDx > PTSD_CUT_OFF)
		{
			logCBT = 0.2887 + (-0.1323*dGender) + (-0.4647*black) + (-0.4502*hispanic) + (-1.2979*otherRace) + (0.1119*age2) + (-0.1129*age3);
			logSPR = 0.6048 + (-0.3331*dGender) + (-0.5914*black) + (-0.4878*hispanic) + (-0.8119*otherRace) + (0.2076*age2) + (-0.0720*age3);
		}
	
		float pCBT = exp(logCBT)/(1+exp(logCBT));
		float pSPR = exp(logSPR)/(1+exp(logSPR));

		float randomPCBT = uniformRealDist();
		priorCBT = (randomPCBT < pCBT) ? 1 : 0;

		float randomPSPR = uniformRealDist();
		priorSPR = (randomPSPR < pSPR) ? 1 : 0;
	}
}


void Agent::countWeeksDisplaced(unsigned short int currTime)
{
	if(currTime == 0)
	{
		//@critera for shelter access
		if(avgHHIncome < INCOME_THRESHOLD && leftHome == LEFT_HOME_YES && incDeclineT0 == INCOME_LOSS_YES)
		{
			if(damage == MODERATE_DAMAGE)
			{
				weeksDisplaced = randomIntegerGenerator(WEEKS_DISPLACED_MIN_MODERATE, WEEKS_DISPLACED_MAX_MODERATE);
			}
			else if(damage == SEVERE_DAMAGE)
			{
				weeksDisplaced = randomIntegerGenerator(WEEKS_DISPLACED_MIN_SEVERE, WEEKS_DISPLACED_MAX_SEVERE);
			}
		}
		else if((avgHHIncome >= INCOME_THRESHOLD && leftHome == LEFT_HOME_YES && incDeclineT0 == INCOME_LOSS_YES) || (leftHome == LEFT_HOME_YES && incDeclineT0 == INCOME_LOSS_NO))
		{
			weeksDisplaced = randomIntegerGenerator(WEEKS_DISPLACED_MIN_T2, WEEKS_DISPLACED_MAX_T2);
		}
	}
	else if(currTime > 0 && leftHome == LEFT_HOME_YES)
	{
		weeksDisplaced--;
		if(weeksDisplaced == 0)
		{
			leftHome = LEFT_HOME_NO;
		}
	}
}

void Agent::ptsdScreeningSteppedCare(unsigned short int currTime)
{
	if(_world->steppedCare && currTime == _world->ptsdScreeningTime)
	{
		//@Step1: initial screening of PTSD on Day 14 after hurricane sandy
		if(initPTSDx > _world->ptsdxCutoff)
		{
			float randomP1 = uniformRealDist();
			if(randomP1 < _world->sensitivity){cbtReferred = true; sprReferred = false;}
			else if(randomP1 < 1.0) {cbtReferred = false; sprReferred = true;}
		}
		else if(initPTSDx <= _world->ptsdxCutoff)
		{
			float randomP2 = uniformRealDist();
			float specficity = 1.0 - _world->specificity;
			if(randomP2 < specficity){cbtReferred = true; sprReferred = false;}
			else if(randomP2 < 1.0){cbtReferred = false; sprReferred = true;}
		}
	}
}


void Agent::updateAgentState(unsigned short int trtmentType)
{
	if(leftHome == LEFT_HOME_YES && incDecline[trtmentType] == INCOME_LOSS_YES)
	{
		if(PTSDstatus[trtmentType] == POSITIVE ){state[trtmentType] = WORST;}
		else if(PTSDstatus[trtmentType] == NEGATIVE){state[trtmentType] = AVERAGE;}
	}
	else if(leftHome == LEFT_HOME_YES && incDecline[trtmentType] == INCOME_LOSS_NO)
	{
		if(PTSDstatus[trtmentType] == POSITIVE){state[trtmentType] = POOR;}
		else if(PTSDstatus[trtmentType] == NEGATIVE){state[trtmentType] = VERY_GOOD;}
	}
	else if(leftHome == LEFT_HOME_NO && incDecline[trtmentType] == INCOME_LOSS_YES)
	{
		if(PTSDstatus[trtmentType] == POSITIVE){state[trtmentType] = VERY_POOR;}
		else if(PTSDstatus[trtmentType] == NEGATIVE){state[trtmentType] = GOOD;}
	}
	else if(leftHome == LEFT_HOME_NO && incDecline[trtmentType] == INCOME_LOSS_NO)
	{
		if(PTSDstatus[trtmentType] == POSITIVE){state[trtmentType] = BELOW_AVERAGE;}
		else if(PTSDstatus[trtmentType] == NEGATIVE){state[trtmentType] = EXCELLENT;}
	}
}



void Agent::ptsdTreatment(unsigned short int currTime, unsigned short int trtmentType)
{
	//if((lastCBT && !lastSPR) || (!lastCBT && lastSPR)){ timeElapsed += 1;}
	if(currTime >= _world->ptsdScreeningTime)
	{
		if(initPTSDx > _world->ptsdxCutoff && curPTSDx[trtmentType] > _world->ptsdxCutoff)
		{
			//@stepped care treatment
			if(_world->steppedCare && (trtmentType == STEPPED_CARE_WITH_SS || trtmentType == STEPPED_CARE_WITHOUT_SS))
			{
				//@80 % positive PTSD cases referred to CBT treatment
			
				if(cbtReferred && !sprReferred && curPTSDx[trtmentType] > _world->ptsdxCutoff)
				{
					provideCognitiveBehaviorTherapy(_world->trtmentTime, currTime, trtmentType, FLAG_CBT_CASES, FLAG_SPR_CASES);
				}
				//@20 % positive PTSD cases referred to SPR treatment only
				else if(!cbtReferred && sprReferred && curPTSDx[trtmentType] > _world->ptsdxCutoff)
				{
					provideSkillForPyschologicalRecovery(_world->trtmentTime, currTime, trtmentType, FLAG_SPR_CASES);
				}
			}
			//@Usual care treatment
			else if(_world->usualCare && trtmentType == USUAL_CARE_WITHOUT_SS)
			{
				provideSkillForPyschologicalRecovery(_world->trtmentTime, currTime, trtmentType, FLAG_SPR_CASES);
			}
		}
		else if(initPTSDx <= _world->ptsdxCutoff && curPTSDx[trtmentType] != 0)
		{
			if(_world->steppedCare && (trtmentType == STEPPED_CARE_WITH_SS || trtmentType == STEPPED_CARE_WITHOUT_SS))
			{
				if(cbtReferred && !sprReferred)
				{
					provideCognitiveBehaviorTherapy(CBT_TIME_NON_CASES + _world->ptsdScreeningTime, currTime, trtmentType, FLAG_CBT_NON_CASES, FLAG_SPR_NON_CASES);

				}
				else if(!cbtReferred && sprReferred)
				{
					provideSkillForPyschologicalRecovery(_world->trtmentTime, currTime, trtmentType, FLAG_SPR_NON_CASES);
				}
			}
			else if(_world->usualCare && trtmentType == USUAL_CARE_WITHOUT_SS)
			{
				provideSkillForPyschologicalRecovery(_world->trtmentTime, currTime, trtmentType, FLAG_SPR_NON_CASES);
			}
		}

		
		if(_world->steppedCare && trtmentType == STEPPED_CARE_WITH_SS)
		{
			if(incDecline[trtmentType] == INCOME_LOSS_YES)
			{
				if(avgHHIncome < INCOME_THRESHOLD) //&& currTime < _world->ptsdScreeningTime + _world->socialWorkerTime)
				{
					//checks if it is agent's first visit ko cbt or spr treatment
					if((currCBT[trtmentType] || currSPR[trtmentType]) && (sessionsCBT[trtmentType] == 0 && sessionsSPR[trtmentType] == 0))
					{
						financialHelpType = EXPEDITE;
						_world->countExpedite += 1;
						reimburseTime[trtmentType] = currTime+randomIntegerGenerator(EXPEDITE_WEEKS_MIN, EXPEDITE_WEEKS_MAX)+WAIT_TIME+randomIntegerGenerator(EXPEDITE_WEEKS_MIN, EXPEDITE_WEEKS_MAX);
					}
				}
				else if(avgHHIncome > INCOME_THRESHOLD)
				{
					if((currCBT[trtmentType] || currSPR[trtmentType]) && (sessionsCBT[trtmentType] == 0 && sessionsSPR[trtmentType] == 0))
					{
						financialHelpType = NORMAL;
						_world->countNormal += 1;
						//reimburseTime = currTime+randomIntegerGenerator(NORMAL_WEEKS_MIN,NORMAL_WEEKS_MAX);
						reimburseTime[trtmentType] = currTime+randomIntegerGenerator(NORMAL_WEEKS_MIN,NORMAL_WEEKS_MAX)+WAIT_TIME+randomIntegerGenerator(NORMAL_WEEKS_MIN,NORMAL_WEEKS_MAX);
					}
				}
			}
		}
		else if((_world->steppedCare && trtmentType == STEPPED_CARE_WITHOUT_SS) || (_world->usualCare && trtmentType == USUAL_CARE_WITHOUT_SS))
		{
			if(incDecline[trtmentType] == INCOME_LOSS_YES)
			{
				if((currCBT[trtmentType] || currSPR[trtmentType]) && (sessionsCBT[trtmentType] == 0 && sessionsSPR[trtmentType] == 0))
				{
					//_world->countNormalNoSW += 1;
					reimburseTime[trtmentType] = currTime+randomIntegerGenerator(NORMAL_WEEKS_MIN,NORMAL_WEEKS_MAX)+WAIT_TIME+randomIntegerGenerator(NORMAL_WEEKS_MIN,NORMAL_WEEKS_MAX);
				}
			}
		}
	}
}



void Agent::ptsdSymptomResolution(unsigned short int currTime, unsigned short int trtmentType)
{
	if(_world->steppedCare || _world->usualCare)
	{
		float trtmentPotency = _world->potency[2*(incDecline[trtmentType]-1)+leftHome];
		if(currCBT[trtmentType] && !currSPR[trtmentType])
		{
			if(curPTSDx[trtmentType] > _world->ptsdxCutoff && trtmentType == STEPPED_CARE_WITH_SS){_world->countCBT[incDeclineT0-1] += 1;}

			curPTSDx[trtmentType] = curPTSDx[trtmentType] - ((trtmentPotency*_world->cbtCoeff)/_world->maxSessionsCBT)*curPTSDx[trtmentType];
			sessionsCBT[trtmentType] += 1;
			currCBT[trtmentType] = false;
		}
		else if(!currCBT[trtmentType] && currSPR[trtmentType])
		{
			if(curPTSDx[trtmentType] > _world->ptsdxCutoff && trtmentType == STEPPED_CARE_WITH_SS){_world->countSPR[incDeclineT0-1] += 1;}

			curPTSDx[trtmentType] = curPTSDx[trtmentType] - ((trtmentPotency*_world->sprCoeff)/_world->maxSessionsSPR)*curPTSDx[trtmentType];
			sessionsSPR[trtmentType] += 1;
			currSPR[trtmentType] = false;
		} 
		else if(!currCBT[trtmentType] && !currSPR[trtmentType])
		{
			float randomP = uniformRealDist();
			if(randomP < PROP_NATURAL_DECAY)
			{
				if(curPTSDx[trtmentType] > _world->ptsdxCutoff && trtmentType == STEPPED_CARE_WITH_SS){_world->countND[incDeclineT0-1] += 1;}

				curPTSDx[trtmentType] = curPTSDx[trtmentType] - ((trtmentPotency*_world->natDecayCoeff)/ND_MONTHS_TO_WEEKS)*curPTSDx[trtmentType];
				//PTSDx = PTSDx - (_world->natDecayCoeff/ND_MONTHS_TO_WEEKS)*PTSDx;
			}
		}

		if(curPTSDx[trtmentType] < 0) {curPTSDx[trtmentType] = 0;}

		if(curPTSDx[trtmentType] > _world->ptsdxCutoff)
		{
			PTSDstatus[trtmentType] = POSITIVE;
			ptsdResolved[trtmentType] = false;
			//ptsdTime += 1;
		}
		else if(curPTSDx[trtmentType] <= _world->ptsdxCutoff)
		{
			PTSDstatus[trtmentType] = NEGATIVE;
			ptsdResolved[trtmentType] = true;
		}
	}
}



void Agent::financialReimbursement(unsigned short int currTime, unsigned short int trtmentType)
{
	if(currTime == reimburseTime[trtmentType] && currTime > 0)
	{
		//incomeDecline = INCOME_LOSS_NO;
		if(damage == SEVERE_DAMAGE) 
		{
			incDecline[trtmentType] = INCOME_LOSS_NO;
		}
		else if(damage == MODERATE_DAMAGE)
		{
			auto its = _file->newYorkSandyFinReimburseMap.equal_range(borough);
			for(auto it = its.first; it != its.second; ++it)
			{
				float randomP = uniformRealDist();
				float pRemimbursement = it->second;

				if(randomP < pRemimbursement)
				{
					incDecline[trtmentType] = INCOME_LOSS_NO;
					break;
				}
				else
				{
					incDecline[trtmentType] = INCOME_LOSS_YES;
					break;
				}
			}
		}	
	}
}

//void Agent::relapsePTSDsymptom(unsigned short int currTime)
//{
//	//if(_world->_steppedCare)
//	//{
//		if(ptsdResolvedSC)
//		{
//			if(initPTSDx > _world->relapsePTSDx)
//			{
//				relapseTimeSC += 1;
//				if(relapseTimeSC > _world->relapseTime && relapseCountSC < _world->numRelapse)
//				{
//					if(uniformRealDist() < PROP_RELAPSE)
//					{
//						PTSDstatus = POSITIVE;
//						curPTSDx = (0.8*initPTSDx);
//						ptsdResolvedSC = false;
//						relapseCountSC += 1;
//					}
//				}
//			}
//		}
//	//}
//}


void Agent::provideCognitiveBehaviorTherapy(unsigned short int maxTrtmentTime, unsigned short int currTime, unsigned short int trtmentType, unsigned short int flagCBT, unsigned short int flagSPR)
{
	float pCBT = calculateTreatmentProbability(flagCBT);
	float randomP = uniformRealDist();

	if(randomP < pCBT && currTime <= maxTrtmentTime)
	{
		currSPR[trtmentType] = false;
		if(sessionsCBT[trtmentType] < 2*(_world->maxSessionsCBT)){currCBT[trtmentType] = true;}
		else if(sessionsCBT[trtmentType] >= 2*(_world->maxSessionsCBT)){currCBT[trtmentType] = false;}
	}
	else
	{
		provideSkillForPyschologicalRecovery(maxTrtmentTime, currTime, trtmentType,  flagSPR);
	}
}

void Agent::provideSkillForPyschologicalRecovery(unsigned short int maxTrtmentTime, unsigned short int currTime, unsigned short int trtmentType, unsigned short int flagSPR)
{
	currCBT[trtmentType] = false;
	float pSPR = calculateTreatmentProbability(flagSPR);
	float randomP = uniformRealDist();
	if(randomP < pSPR && currTime <= maxTrtmentTime)
	{
		if(sessionsSPR[trtmentType] < _world->maxSessionsSPR){currSPR[trtmentType] = true;}
		else if(sessionsSPR[trtmentType] >= _world->maxSessionsSPR) {currSPR[trtmentType] = false;}
	}
	else
	{
		currSPR[trtmentType] = false;
	}
}

float Agent::calculateTreatmentProbability(unsigned short int flag)
{
	//dummy variables related to age, race and gender
	float log = 0.0;
	float p = 0.0;
	int dGender = (gender == MALE) ? 1 : 0;

	//logistic regression for CBT treatment for PTSD cases
	if(flag == FLAG_CBT_CASES)
	{
		log = -2.11 + (-0.1941*dGender) + (-0.5237*black) + (-1.0845*hispanic) + (-0.2653*otherRace) + (-0.1139*age2) + (0.2455*age3) + (1.8377*priorCBT);	
	}
	//logistic regression equation for SPR for PTSD cases
	else if(flag == FLAG_SPR_CASES)
	{
		log = -1.7778 + (0.00136*dGender) + (-0.6774*black) + (-0.8136*hispanic) + (-0.3118*otherRace) + (-0.0549*age2) + (0.3746*age3) + (1.4076*priorSPR);
	}
	//logistic regression for CBT treatment for non PTSD cases
	else if(flag == FLAG_CBT_NON_CASES)
	{
		log = -4.1636 + (-0.6422*dGender) + (-0.8040*black) + (-0.6795*hispanic) + (0.1976*otherRace) + (0.1453*age2) + (-0.4203*age3) + (2.4109*priorCBT);
	}
	//logistic regression equation for SPR for non PTSD cases
	else if(flag == FLAG_SPR_NON_CASES)
	{
		log = -3.7355 + (-0.5319*dGender) + (-1.2562*black) + (-0.4632*hispanic) + (0.1427*otherRace) + (0.1703*age2) + (-0.5289*age3) + (2.0696*priorSPR);
	}

	p = exp(log)/(1+exp(log)); 
	return p;
}


int Agent::randomIntegerGenerator(int min, int max)
{
	static boost::mt19937 rng(time(NULL));

	boost::random::uniform_int_distribution<>dist(min, max);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_int_distribution<>>generator(rng, dist);

	int randVal = generator();

	return randVal;
}

float Agent::uniformRealDist()
{
	static boost::mt19937 rng(time(NULL)); http://stackoverflow.com/questions/2254909/boost-random-number-generator

	boost::random::uniform_real_distribution<>dist(0, 1);
	boost::random::variate_generator<boost::mt19937&, boost::random::uniform_real_distribution<>>generator(rng, dist);

	float randVal = generator();

	return randVal;

}


unsigned int Agent::getID() const 
{
	return ID;
}

unsigned short int Agent::getKey() const
{
	return key;
}

unsigned short int Agent::getAgeCat() const 
{ 
	return ageCat;
}

unsigned short int Agent::getAge() const
{
	return age;
}

unsigned short int Agent::getRace() const
{ 
	return race;
}

unsigned short int Agent::getGender() const
{ 
	return gender;
}

unsigned short int Agent::getYearEmployed() const
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

unsigned short int Agent::getBorough() const
{ 
	return borough;
}

unsigned short int Agent::getHHsize() const
{ 
	return hhSize;
}
unsigned short int Agent::getDamage() const
{
	return damage;
}

unsigned short int Agent::getElectricLoss() const 
{ 
	return lossElectric;
}

unsigned short int Agent::getHeatLoss() const 
{ 
	return lossHeat;
}

unsigned short int Agent::getWaterLoss() const 
{ 
	return lossWater;
}

//int Agent::getFinancialLoss() const
//{
//	return lossFinancial;
//}

unsigned short int Agent::getFinancialLossType() const
{
	return lossFinancialType;
}

int Agent::getFinancialLossAmnt() const
{
	return lossFinancialAmnt;
}

unsigned short int Agent::getIncomeDecline() const
{
	return incDecline[STEPPED_CARE_WITH_SS];
}

unsigned short int Agent::getIncomeDeclineT0() const
{
	return incDeclineT0;
}

unsigned short int Agent::getLeftHome() const
{
	return leftHome;
}

unsigned short int Agent::getLeftHomeT0() const
{
	return leftHomeT0;
}

unsigned short int Agent::getPTSDstatus() const
{
	return PTSDstatus[STEPPED_CARE_WITH_SS];
}

float Agent::getPTSDsymptom() const
{
	return curPTSDx[STEPPED_CARE_WITH_SS];
	//return curPTSDx;
}

float Agent::getPTSDsymptomNoSW() const
{
	return curPTSDx[STEPPED_CARE_WITHOUT_SS];
	//return curPTSDxNoSW;
}

float Agent::getPTSDsymptomUC() const
{
	return curPTSDx[USUAL_CARE_WITHOUT_SS];
	//return curPTSDxUC;
}

float Agent::getInitialPTSDsymptom() const
{
	return initPTSDx;
}

bool Agent::getPTSDresolvedSC() const
{
	return ptsdResolved[STEPPED_CARE_WITH_SS];
	//return ptsdResolvedSC;
}

bool Agent:: getPTSDresolvedSCNoSW() const
{
	return ptsdResolved[STEPPED_CARE_WITHOUT_SS];
	//return ptsdResolvedSCNoSW;
}

bool Agent::getPTSDresolvedUC() const
{
	return ptsdResolved[USUAL_CARE_WITHOUT_SS];
	//return ptsdResolvedUC;
}

unsigned short int Agent::getAgentState() const
{
	return state[STEPPED_CARE_WITH_SS];
	//return state;
}

unsigned short int Agent::getAgentStateNoSW() const
{
	return state[STEPPED_CARE_WITHOUT_SS];
	//return stateNoSW;
}

unsigned short int Agent::getAgentStateUC() const
{
	return state[USUAL_CARE_WITHOUT_SS];
	//return stateUC;
}

unsigned short int Agent::getFinancialHelpType() const
{
	return financialHelpType;
}

void Agent::setAvgHHIncome(float avgFamilyIncome)
{
	this->avgHHIncome = avgFamilyIncome;
}

void Agent::setAvgHHSaving(float avgFamilySaving)
{
	this->avgHHSaving = avgFamilySaving;
}

void Agent::setDamage(unsigned short int houseDamage)
{
	this->damage = houseDamage;
}

void Agent::setElectricLoss(unsigned short int electricLoss)
{
	this->lossElectric = electricLoss;
}

void Agent::setHeatLoss(unsigned short int heatLoss)
{
	this->lossHeat = heatLoss;
}

void Agent::setWaterLoss(unsigned short int waterLoss)
{
	this->lossWater = waterLoss;
}

//void Agent::setFinancialLoss(int financialLoss)
//{
//	this->lossFinancial = financialLoss;
//}

void Agent::setFinancialLossType(unsigned short int financialLossType)
{
	this->lossFinancialType = financialLossType;
}

void Agent::setFinancialLossAmnt(int financialLossAmnt)
{
	this->lossFinancialAmnt = financialLossAmnt;
}

void Agent::setIncomeDecline(unsigned short int incDecline)
{
	for(unsigned short int i = STEPPED_CARE_WITH_SS; i <= USUAL_CARE_WITHOUT_SS; i++)
	{
		this->incDecline[i] = incDecline;
	}	
	
	this->incDeclineT0 = incDecline;
}

void Agent::setLeftHome(unsigned short int leftHouse)
{
	this->leftHome = leftHouse;
	this->leftHomeT0 = leftHouse;
}

void Agent::setPTSDstatus(unsigned short int statusPTSD)
{
	for(unsigned short int i = STEPPED_CARE_WITH_SS; i <= USUAL_CARE_WITHOUT_SS; i++)
	{
		this->PTSDstatus[i] = statusPTSD;
	}	
}

void Agent::setPTSDsymptom(float inPTSDx)
{
	for(unsigned short int i = STEPPED_CARE_WITH_SS; i <= USUAL_CARE_WITHOUT_SS; i++)
	{
		this->curPTSDx[i] = inPTSDx;
	}	
	this->initPTSDx = inPTSDx;
	
}







