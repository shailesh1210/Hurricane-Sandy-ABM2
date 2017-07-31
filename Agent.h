#ifndef __Agent_h__
#define __Agent_h__

#include <iostream>
#include <memory>
#include <random>
#include <cmath>
#include <ctime>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/container/vector.hpp>
#include <boost/array.hpp>

#define NUM_TREATMENT 3
#define STEPPED_CARE_WITH_SS 0
#define STEPPED_CARE_WITHOUT_SS 1
#define USUAL_CARE_WITHOUT_SS 2

class FileHandling;
class Skeleton;

class Agent
{
public:
	Agent(Skeleton *world, FileHandling *file);
	virtual ~Agent();

	
	void setAgentAttributes(unsigned int idx);
	void executeAgentRules(unsigned short int currTime);

	unsigned int getID() const;
	unsigned short int getKey() const;
	unsigned short int getAgeCat() const;
	unsigned short int getAge() const;
	unsigned short int getRace() const;
	unsigned short int getGender() const;
	unsigned short int getYearEmployed() const;
	int getIncome() const;
	int getSubIncome() const;
	int getIndvIncome() const;
	float getAvgHHIncome() const;
	float getAvgHHSaving() const;
	unsigned short int getBorough() const;
	unsigned short int getHHsize() const;
	unsigned short int getDamage() const;
	unsigned short int getElectricLoss() const;
	unsigned short int getHeatLoss() const;
	unsigned short int getWaterLoss() const;
	//int getFinancialLoss() const;
	unsigned short int getFinancialLossType() const;
	int getFinancialLossAmnt() const;
	unsigned short int getIncomeDecline() const;
	unsigned short int getIncomeDeclineT0() const;
	unsigned short int getLeftHome() const;
	unsigned short int getLeftHomeT0() const;
	unsigned short int getPTSDstatus() const;
	float getPTSDsymptom() const;
	float getPTSDsymptomNoSW() const;
	float getPTSDsymptomUC() const;
	float getInitialPTSDsymptom() const;
	bool getPTSDresolvedSC() const;
	bool getPTSDresolvedSCNoSW() const;
	bool getPTSDresolvedUC() const;
	unsigned short int getAgentState() const;
	unsigned short int getAgentStateNoSW() const;
	unsigned short int getAgentStateUC() const;
	unsigned short int getFinancialHelpType() const;


	void setAvgHHIncome(float avgFamilyIncome);
	void setAvgHHSaving(float avgFamilySaving);
	void setDamage(unsigned short int houseDamage);
	void setElectricLoss(unsigned short int electricLoss);
	void setHeatLoss(unsigned short int heatLoss);
	void setWaterLoss(unsigned short int waterLoss);
	//void setFinancialLoss(int financialLoss);
	void setFinancialLossType(unsigned short int financialLossType);
	void setFinancialLossAmnt(int financialLossAmnt);
	void setIncomeDecline(unsigned short int incDecline);
	void setLeftHome(unsigned short int leftHouse);
	void setPTSDstatus(unsigned short int statusPTSD);
	void setPTSDsymptom(float inPTSDx);


protected:
	//methods to assign age, race and gender to agents in different boroughs
	void ageRaceGenderBoroDist();
	void householdSizeDist();
	//method to assign house income to agents
	void houseIncomeDist();
	void houseSubIncomeDist();

	void predictPriorTrtmentEnrollment(unsigned short int currTime);
	void countWeeksDisplaced(unsigned short int currTime);
	void ptsdScreeningSteppedCare(unsigned short int currTime);

	void updateAgentState(unsigned short int trtmentType);
	void ptsdTreatment(unsigned short int currTime, unsigned short int trtmentType);
	void ptsdSymptomResolution(unsigned short int currTime, unsigned short int trtmentType);
	void financialReimbursement(unsigned short int currTime, unsigned short int trtmentType);

	void provideCognitiveBehaviorTherapy(unsigned short int maxTrtmentTime, unsigned short int currTime, unsigned short int trtmentType, unsigned short int flagCBT, unsigned short int flagSPR);
	void provideSkillForPyschologicalRecovery(unsigned short int maxTrtmentTime, unsigned short int currTime, unsigned short int trtmentType, unsigned short int flagSPR);

	float calculateTreatmentProbability(unsigned short int flag);
	int randomIntegerGenerator(int min, int max);
	float uniformRealDist();

protected:
//public:
	
	std::shared_ptr<Skeleton>_world;
	std::shared_ptr<FileHandling> _file;
	
	unsigned int ID;
	unsigned short int key;
	unsigned short int ageCat, age, race, gender; 
	int income; 
	unsigned short int borough; //agent's demographic variables
	unsigned short int yrsEmployed; // duration of employment
	int subInc; //agent's household income based on 7 categories of income
	int indIncome; //agent's individual income, randomly assigned based on sub-income
	float avgHHIncome; //agent's average household income
	float avgHHSaving; // agent's average household saving
	unsigned short int hhSize;		//agent's household size
	unsigned short int damage; //indicator for level of damage by Hurricane Sandy on agent's residence 0-no damage 1-moderate 2-severe
	unsigned short int lossElectric; //indicator of electricity loss 0- didn't lose electricity 1- lost electricity
	unsigned short int lossHeat; //indicator of Heat loss 0- didn't lose Heat 1- lost Heat
	unsigned short int lossWater; //indicator of Water loss 0- didn't lose Water 1- lost Water
	unsigned short int lossFinancialType; //indicatior of type of financial loss
	int lossFinancialAmnt; //indicator of amount of money lost due to damage to the home
	boost::array<unsigned short int, NUM_TREATMENT>incDecline;
	unsigned short int incDeclineT0;
	unsigned short int leftHome, leftHomeT0; //indicates whether agent left home or not due to damage caused by Sandy. 0-Didnot leave home 1 - left home
	boost::array<unsigned short int, NUM_TREATMENT>PTSDstatus; //indicates whether agent has positive PTSD case status or not based on whether it left home
	boost::array<float, NUM_TREATMENT>curPTSDx; //number of PTSD symptom count
	float initPTSDx; //PTSD symptom count at initial time step
	boost::array<unsigned short int, NUM_TREATMENT> state; //agent's mental health state

	unsigned short int weeksDisplaced; //number of days displaced from home

	//treatment variables
	unsigned short int priorCBT;
	unsigned short int priorSPR;

	//outcome of screening with sen = 0.8 and sp = 0.8
	bool cbtReferred;
	bool sprReferred;

	boost::array<bool, NUM_TREATMENT> currCBT, currSPR; //indictes whether agent received CBT/SPR treatment at current time-step
	boost::array<unsigned short int, NUM_TREATMENT> sessionsCBT, sessionsSPR;
	boost::array<unsigned short int, NUM_TREATMENT> ptsdResolved;
	unsigned short int financialHelpType;
	boost::array<unsigned short int, NUM_TREATMENT> reimburseTime;
	
	//dummy variables
	unsigned short int age1, age2, age3;
	unsigned short int white, black, hispanic, otherRace;
};

#endif __Agent_h__