#pragma once

#ifndef __Parameters_h__
#define __Parameters_h__

#include <iostream>
#include <vector>
#include <algorithm>
#include <list>
#include <memory>
#include <unordered_map>

#include <boost/unordered_map.hpp>
#include <boost/container/list.hpp>
#include <boost/container/vector.hpp>
//#include <boost/fusion/include/pair.hpp>

#define AGE1 1 //age 18 - 34
#define AGE1_MIN 18
#define AGE1_MAX 34

#define AGE2 2 //age 35 - 64
#define AGE2_MIN 35
#define AGE2_MAX 64

#define AGE3 3 //age 65+
#define AGE3_MIN 65
#define AGE3_MAX 70

#define WHITE 1 //race - White non Hispanic
#define BLACK 2 //race - Black non Hispanic
#define OTHER 3 //race - Other non Hispanic
#define HISP 4 //race - Hispanics

#define INCOME1 1 //income 0 - 40K 
#define INCOME2 2 //income 40-80K
#define INCOME3 3 //income 80K+

#define INCOME1_MIN 0 
#define INCOME2_MIN 40000
#define INCOME3_MIN 80000 
#define INCOME3_MAX 500000

#define INCOME_THRESHOLD 60000

#define SUBINC1 1 // income < 20K
#define SUBINC2 2 // income 20-40K
#define SUBINC3 3 //income 40-60K
#define SUBINC4 4 //income 60 - 80K
#define SUBINC5 5 // income 80-100K
#define SUBINC6 6  // income 100-150K
#define SUBINC7 7  //income 150K+

#define MALE 0
#define FEMALE 1

#define HHsize1 1
#define HHsize2 2
#define HHsize3 3
#define HHsize4 4

#define BRONX 1
#define BROOKLYN 2
#define MANHATTAN 3
#define QUEENS 4
#define STATEN 5

//Sandy damage parameters
#define NO_DAMAGE 1
#define MODERATE_DAMAGE 2
#define SEVERE_DAMAGE 3

// Utility parameters
#define ELECTRIC_LOSS 1
#define NO_ELECTRIC_LOSS 0

#define HEAT_LOSS 1
#define NO_HEAT_LOSS 0

#define WATER_LOSS 1
#define NO_WATER_LOSS 0

//Evacuation parameters
#define LEFT_HOME_YES 1
#define LEFT_HOME_NO 0

//financial loss parameters
#define INCOME_LOSS_YES 2
#define INCOME_LOSS_NO 1

#define FINANCIAL_LOSS_TYPE1 1 //financial loss in the range of 0-10K
#define FINANCIAL_LOSS_TYPE2 2 //financial loss in the range of 10-25K
#define FINANCIAL_LOSS_TYPE3 3 //financial loss greater than 25K

#define MIN_LOSS_TYPE3 25000
#define MAX_LOSS_TYPE3 40000

#define MAX_PEOPLE_HHSIZE4 6

#define NUM_BOROUGHS 5


//definition of agent's mental health state
#define EXCELLENT 0 // LeftHome = False, IncomeDecline = False; PTSD status = -ve
#define VERY_GOOD 1 // LeftHome = True, IncomeDecline = False; PTSD status = -ve
#define GOOD 2 // LeftHome = False, IncomeDecline = True; PTSD status = -ve
#define AVERAGE 3 // LeftHome = True, IncomeDecline = True; PTSD status = -ve
#define BELOW_AVERAGE 4 // LeftHome = False, IncomeDecline = False; PTSD status = +ve
#define POOR 5 //LeftHome = True, IncomeDecline = False; PTSD status = +ve
#define VERY_POOR 6 //LeftHome = False, IncomeDecline = True; PTSD status = +ve
#define WORST 7 // LeftHome = True, IncomeDecline = True; PTSD status = +ve

//displacement parameters
#define WEEKS_DISPLACED_MIN_MODERATE 4//30
#define WEEKS_DISPLACED_MAX_MODERATE 8//60

#define WEEKS_DISPLACED_MIN_SEVERE 8//60
#define WEEKS_DISPLACED_MAX_SEVERE 14//100

#define WEEKS_DISPLACED_MIN_T2 1//2
#define WEEKS_DISPLACED_MAX_T2 4//14

//PTSD parameters
#define POSITIVE 1
#define NEGATIVE 0

//Financial help parameters
#define EXPEDITE 2
#define NORMAL 1

#define WAIT_TIME 12

#define EXPEDITE_WEEKS_MIN 1//4//30//30//30
#define EXPEDITE_WEEKS_MAX 3//8//12//90//90//60

#define NORMAL_WEEKS_MIN 8//8//12//90//90//60
#define NORMAL_WEEKS_MAX 16//16//64//450//450//120

//natural decay parameters

#define PROP_NATURAL_DECAY 0.3
#define ND_MONTHS_TO_WEEKS 12//84

#define CBT_TIME_NON_CASES 3//21
#define PTSD_CUT_OFF 6.0
#define PROP_RELAPSE 0.15

#define FLAG_CBT_CASES 1
#define FLAG_SPR_CASES 2
#define FLAG_CBT_NON_CASES 3
#define FLAG_SPR_NON_CASES 4


struct Borough
{
	float pBorough;
	unsigned short int inBorough;
};

struct Demographics
{
	float pDemoAtt;
	unsigned short int inBorough, inAge, inRace, inGender;
};

struct Household
{
	float pHHsize;
	unsigned short int inHHSize, inBoro;
};

struct Income
{
	float pIncome;
	unsigned short int inHHsize, inBoro; 
	int inIncome;

};

struct SubIncome
{
	float pSubInc;
	int inIncome, inSubIncome; 
	unsigned short int inBoro;
	int minIncome, maxIncome;
};

struct Expense
{
	unsigned short int inHHsize; 
	int minIncome;
	int maxIncome;
	float ratioExpenseInc;
};

struct SandyDamage
{
	float pSandyDamage;
	unsigned short int inDamage; 
	int inIncome; 
	unsigned short int inBoro;
};

struct SandyFinancialLoss
{
	float pFinLossType;
	unsigned short int inBorough, inFinLossType; 
	int minLossAmnt, maxLossAmnt;
};

struct SandyPTSDstatus
{
	float pSandyPTSD;
	unsigned short int inIncomeLoss;
	unsigned short int inGender;
	unsigned short int inAgeCat;
	unsigned short int inPTSDstatus;
	//unsigned short int inLeftHome, inPTSDstatus;
};

struct SandyPTSDx
{
	unsigned short int keyPTSDx;
	float inPTSDx;
};

struct SandyFinancialReimbursement
{
	float pReimbursement;
	unsigned short int inBorough;
};


class Agent;
class Family;

typedef boost::container::vector<Borough> BoroughVector;
typedef boost::unordered_multimap<unsigned short int, Demographics> DemographMap;
typedef boost::unordered_multimap<unsigned short int, Household> HouseHoldMap;
typedef boost::unordered_multimap<unsigned short int, Income> IncomeMap;
typedef boost::unordered_multimap<unsigned short int, SubIncome> SubIncomeMap;
typedef boost::unordered_multimap<unsigned short int, Expense> ExpenseMap;
typedef boost::unordered_multimap<unsigned short int, SandyDamage> SandyDamageMap;
typedef boost::unordered_multimap<unsigned short int, SandyFinancialLoss> SandyFinLossMap;
typedef boost::unordered_multimap<unsigned short int, SandyPTSDstatus> SandyPTSDstatusMap;
typedef boost::unordered_multimap<unsigned short int, float> SandyPTSDxMap;
typedef boost::unordered_multimap<unsigned short int, float> SandyFinReimburseMap;

typedef boost::unordered_multimap<unsigned short int, Agent> AgentMap;

//typedef std::vector <Agent> AgentList;

typedef boost::container::vector <Agent> AgentList;
typedef boost::container::vector <Family> FamilyList;


//typedef std::vector<Agent> AgentVector;

//typedef std::vector <Agent> AgentList;
//typedef std::vector <Family> FamilyList;


#endif __Parameters_h__