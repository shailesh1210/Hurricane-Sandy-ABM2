#ifndef FAMILY_H
#define FAMILY_H

#include "Parameters.h"


class Skeleton;
class Agent;
class FileHandling;

class Family
{
public:
	Family(int index);
	virtual ~Family();
	
	void setFamilyAttributes(Agent a);
	void addAgent(Agent a);

	void calcAverageFamilyIncome();	
	void calcAverageFamilySaving(FileHandling *file);
	//method to predict damages caused by Hurricane Sandy
	
	void predictSandyDamage(FileHandling *file);

	int getFamilyID() const;
	int getFamilyKey() const;
	int getFamilyRace() const;
	int getFamilyIncome() const;
	int getFamilyBorough() const;
	int getFamilySize() const;
	int getHouseDamage() const;
	int getFinancialLossType() const;
	int getFinancialLossAmnt() const;
	float getAvgFamilyIncome() const;
	float getAvgFamilySaving() const;
	float getAvgEmploymentDuration() const;

	AgentList _agentFamList;

protected:
	void setDummyVariables();
	
	//method to predict electricity, heat and water loss 
	void predictElectricLoss();
	void predictHeatLoss();
	void predictWaterLoss();
	//method to predict to the income loss based on the damage
	void predictEconomicLoss(FileHandling *file);

	//method to predict whether agent left home or not due to damage caused by Hurricane Sandy
	int predictEvacuation(Agent a);
	int predictPTSDstatus(FileHandling *file, int leftHome, double &PTSDx, int &incDecline);
	

	double uniformRealDist();
	int randomFinancialLossGenerator(int min, int max);

protected:

	int familyID;
	int familyKey;

	//variables to indicate the damages and losses suffered by families
	int houseDamage; //1-No damage 2-Moderate damage 3-Severe damage
	int electricLoss; // 1-Electric Loss 0 - No electric loss
	int heatLoss; // 1- Heat Loss 0- No heat loss
	int waterLoss; // 1-Water loss 0-No water loss
	//int financialLoss; // 1-income loss 0-No income loss
	int financialLossType; //1-Loss in the range of 0-10K; 2-Loss in the range of 10K-25K; 3-Loss more than 25K 
	int financialLossAmnt;

	int familyRace;
	int familyIncome;
	int familyBorough;
	int familySize;
	float avgFamilyIncome;
	float avgEmploymentDuration;
	float avgIncomeSaved;

	int bronx, brooklyn, manhattan, queens, staten; //dummy variables related to boroughs
	int noDamage, modDamage, severeDamage; //dummy variables related to damage
	
	
};
#endif FAMILY_H