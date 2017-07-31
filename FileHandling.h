#ifndef __FileHandling_h__
#define __FileHandling_h__

#include <iostream>
#include <fstream>
#include "Parameters.h"

class FileHandling
{
public:
	FileHandling();
	virtual ~FileHandling();

	void readInputFile();
	
protected:
	void readSandyDamageFile();
	void readExpenseFile();

	void readBoroughFileNYC();
	void readDemoFileNYC();
	void readHHsizeFileNYC();
	void readIncomeFileNYC();
	void readSubIncomeFileNYC();
	void readSandyDamagePredictorFileNYC();
	void readSandyFinancialLossFileNYC();
	void readSandyPTSDFileNYC();
	void readSandyFinancialReimbursementFileNYC();
	
public:

	//DemoVector newYork;

	/*HouseholdVector _newYorkHHsize;
	IncomeVector _newYorkIncome;
	SubIncomeVector _newYorkIncDiv;
	ExpenseVector _newYorkExpense;
	SandyDamageVector _newYorkSandyDamage;*/

	//SandyFinLossVector _newYorkSandyFinLoss;
	//SandyPTSDvector _newYorkSandyPTSD;

	BoroughVector newYorkBoro;
	DemographMap newYorkMap;
	HouseHoldMap newYorkHHmap;
	IncomeMap newYorkIncMap;
	SubIncomeMap newYorkSubIncMap;
	ExpenseMap newYorkExpenseMap;
	SandyDamageMap newYorkSandyDamageMap;
	SandyFinLossMap newYorkSandyFinLossMap;
	SandyPTSDstatusMap newYorkSandyPTSDMap;
	SandyPTSDxMap newYorkSandyPTSDsympMap;
	SandyFinReimburseMap newYorkSandyFinReimburseMap;
};
#endif __FileHandling_h__