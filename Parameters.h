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

////simulation parameters
//#define NUM_AGENTS 500000
//#define NUM_STEPS 100
//

//struct Borough
//{
//	double pBorough;
//	int inBorough;
//};

struct Demographics
{
	double pDemoAtt;
	int inBorough, inAge, inRace, inGender;
};

struct Household
{
	double pHHsize;
	int inHHSize, inBoro;
};

struct Income
{
	double pIncome;
	int inHHsize, inBoro, inIncome;

};

struct SubIncome
{
	double pSubInc;
	int inIncome, inSubIncome, inBoro;
	int minIncome, maxIncome;
};

struct Expense
{
	int inHHsize; 
	int minIncome;
	int maxIncome;
	double ratioExpenseInc;
};

struct SandyDamage
{
	double pSandyDamage;
	int inDamage, inIncome, inBoro;
};

struct SandyFinancialLoss
{
	double pFinLossType;
	int inBorough, inFinLossType, minLossAmnt, maxLossAmnt;
};

struct SandyPTSD
{
	double pSandyPTSD;
	int inIncomeLoss;
	int inLeftHome, inPTSDstatus;
	double inPTSDx;
};


class Agent;
class Family;
//typedef std::vector <Borough> BoroVector;
//typedef std::vector <Demographics> DemoVector;
typedef boost::container::vector <Demographics> DemoVector;
//typedef std::vector <Household> HouseholdVector;
//typedef std::vector <Income> IncomeVector;
//typedef std::vector <SubIncome> SubIncomeVector;
//typedef std::vector <Expense> ExpenseVector;
//typedef std::vector <SandyDamage> SandyDamageVector;
//typedef std::vector <SandyFinancialLoss> SandyFinLossVector;
//typedef std::vector <SandyPTSD> SandyPTSDvector;


//typedef std::unordered_multimap<int, Household> HouseHoldMap;
//typedef std::unordered_multimap<int, Income> IncomeMap;
//typedef std::unordered_multimap<int, SubIncome> SubIncomeMap;
//typedef std::unordered_multimap<int, Expense> ExpenseMap;
//typedef std::unordered_multimap<int, SandyDamage> SandyDamageMap;
//typedef std::unordered_multimap<int, SandyFinancialLoss> SandyFinLossMap;
//typedef std::unordered_multimap<int, SandyPTSD> SandyPTSDMap;
//
//typedef std::unordered_multimap<int, Agent> AgentMap;

typedef boost::unordered_multimap<int, Household> HouseHoldMap;
typedef boost::unordered_multimap<int, Income> IncomeMap;
typedef boost::unordered_multimap<int, SubIncome> SubIncomeMap;
typedef boost::unordered_multimap<int, Expense> ExpenseMap;
typedef boost::unordered_multimap<int, SandyDamage> SandyDamageMap;
typedef boost::unordered_multimap<int, SandyFinancialLoss> SandyFinLossMap;
typedef boost::unordered_multimap<int, SandyPTSD> SandyPTSDMap;

typedef boost::unordered_multimap<int, Agent> AgentMap;

//
//typedef std::list <Agent> AgentList;
//typedef std::list <Family> FamilyList;

//typedef boost::container::list <Agent> AgentList;
typedef boost::container::vector <Agent> AgentList;
//typedef boost::container::list <Family> FamilyList;
typedef boost::container::vector <Family> FamilyList;

//typedef std::vector<Agent> AgentVector;


#endif __Parameters_h__