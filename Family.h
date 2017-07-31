#ifndef FAMILY_H
#define FAMILY_H

#include "Parameters.h"


class Skeleton;
class Agent;
class FileHandling;

class Family
{
public:
	Family(unsigned int index);
	virtual ~Family();
	
	void setFamilyAttributes(Agent a);
	void addAgent(Agent a);

	void calcAverageFamilyIncome();	
	void calcAverageFamilySaving(FileHandling *file);
	//method to predict damages caused by Hurricane Sandy
	
	void predictSandyDamage(FileHandling *file);

	unsigned int getFamilyID() const;
	unsigned short int getFamilyKey() const;
	unsigned short int getFamilyRace() const;
	int getFamilyIncome() const;
	unsigned short int getFamilyBorough() const;
	unsigned short int getFamilySize() const;
	unsigned short int getHouseDamage() const;
	unsigned short int getFinancialLossType() const;
	int getFinancialLossAmnt() const;
	float getAvgFamilyIncome() const;
	float getAvgFamilySaving() const;
	float getAvgEmploymentDuration() const;

	AgentList agentFamList;

protected:
	void setDummyVariables();
	
	//method to predict electricity, heat and water loss 
	void predictElectricLoss();
	void predictHeatLoss();
	void predictWaterLoss();
	//method to predict to the income loss based on the damage
	void predictEconomicLoss(FileHandling *file);

	//method to predict whether agent left home or not due to damage caused by Hurricane Sandy
	unsigned short int predictEvacuation(unsigned short int ageCat);
	unsigned short int predictPTSDstatus(FileHandling *file, unsigned short int ageCat, unsigned short int gender, unsigned short int leftHome, float &PTSDx, unsigned short int &incDecline);
	
	float uniformRealDist();
	int randomFinancialLossGenerator(int min, int max);

protected:

	unsigned int familyID;
	unsigned short int familyKey;

	//variables to indicate the damages and losses suffered by families
	unsigned short int houseDamage; //1-No damage 2-Moderate damage 3-Severe damage
	unsigned short int electricLoss; // 1-Electric Loss 0 - No electric loss
	unsigned short int heatLoss; // 1- Heat Loss 0- No heat loss
	unsigned short int waterLoss; // 1-Water loss 0-No water loss
	//int financialLoss; // 1-income loss 0-No income loss
	unsigned short int financialLossType; //1-Loss in the range of 0-10K; 2-Loss in the range of 10K-25K; 3-Loss more than 25K 
	int financialLossAmnt;

	unsigned short int familyRace;
	int familyIncome;
	unsigned short int familyBorough;
	unsigned short int familySize;
	float avgFamilyIncome;
	float avgEmploymentDuration;
	float avgIncomeSaved;

	unsigned short int bronx, brooklyn, manhattan, queens, staten; //dummy variables related to boroughs
	unsigned short int noDamage, modDamage, severeDamage; //dummy variables related to damage
	
	
};
#endif FAMILY_H