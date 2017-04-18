#ifndef __Agent_h__
#define __Agent_h__

#include <iostream>
#include <memory>

class FileHandling;

class Agent
{
public:
	Agent(FileHandling *file);
	virtual ~Agent();

	
	void setAgentAttributes(int idx);

	int getID() const;
	int getKey() const;
	int getAgeCat() const;
	int getAge() const;
	int getRace() const;
	int getGender() const;
	int getYearEmployed() const;
	int getIncome() const;
	int getSubIncome() const;
	int getIndvIncome() const;
	float getAvgHHIncome() const;
	float getAvgHHSaving() const;
	int getBorough() const;
	int getHHsize() const;
	int getDamage() const;
	int getElectricLoss() const;
	int getHeatLoss() const;
	int getWaterLoss() const;
	//int getFinancialLoss() const;
	int getFinancialLossType() const;
	int getFinancialLossAmnt() const;
	int getLeftHome() const;
	int getPTSDstatus() const;
	double getPTSDsymptom() const;

	void setAvgHHIncome(float avgFamilyIncome);
	void setAvgHHSaving(float avgFamilySaving);
	void setDamage(int houseDamage);
	void setElectricLoss(int electricLoss);
	void setHeatLoss(int heatLoss);
	void setWaterLoss(int waterLoss);
	//void setFinancialLoss(int financialLoss);
	void setFinancialLossType(int financialLossType);
	void setFinancialLossAmnt(int financialLossAmnt);
	void setIncomeDecline(int incDecline);
	void setLeftHome(int leftHouse);
	void setPTSDstatus(int statusPTSD);
	void setPTSDsymptom(double inPTSDx);

	void decayPTSDsymptom();
	
protected:
	//methods to assign age, race and gender to agents in different boroughs
	void ageRaceGenderBoroDist();
	void householdSizeDist();
	//method to assign house income to agents
	void houseIncomeDist();
	void houseSubIncomeDist();



	int randomIntegerGenerator(int min, int max);
	double uniformRealDist();

protected:
//public:
	
	std::shared_ptr<FileHandling> _file;
	
	int ID;
	int key;
	int ageCat, age, race, gender, income, borough; //agent's demographic variables
	int yrsEmployed; // duration of employment
	int subInc; //agent's household income based on 7 categories of income
	int indIncome; //agent's individual income, randomly assigned based on sub-income
	float avgHHIncome; //agent's average household income
	float avgHHSaving; // agent's average household saving
	int hhSize;		//agent's household size
	int damage; //indicator for level of damage by Hurricane Sandy on agent's residence 0-no damage 1-moderate 2-severe
	int lossElectric; //indicator of electricity loss 0- didn't lose electricity 1- lost electricity
	int lossHeat; //indicator of Heat loss 0- didn't lose Heat 1- lost Heat
	int lossWater; //indicator of Water loss 0- didn't lose Water 1- lost Water
	//int lossFinancial; //indicator of Financial loss due to property damage
	int lossFinancialType; //indicatior of type of financial loss
	int lossFinancialAmnt; //indicator of amount of money lost due to damage to the home
	int incDecline; //indictor of whether agent suffered financial loss or not
	int leftHome; //indicates whether agent left home or not due to damage caused by Sandy. 0-Didnot leave home 1 - left home
	int PTSDstatus; //indicates whether agent has positive PTSD case status or not based on whether it left home
	double PTSDx; //number of PTSD symptom count
};

#endif __Agent_h__