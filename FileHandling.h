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
	void readSandyDamageFile();
	void readExpenseFile();
protected:
	void readDemoFileNYC();
	void readHHsizeFileNYC();
	void readIncomeFileNYC();
	void readSubIncomeFileNYC();
	void readSandyDamagePredictorFileNYC();
	void readSandyFinancialLossFileNYC();
	void readSandyPTSDFileNYC();
	
public:

	DemoVector _newYork;

	/*HouseholdVector _newYorkHHsize;
	IncomeVector _newYorkIncome;
	SubIncomeVector _newYorkIncDiv;
	ExpenseVector _newYorkExpense;
	SandyDamageVector _newYorkSandyDamage;*/

	//SandyFinLossVector _newYorkSandyFinLoss;
	//SandyPTSDvector _newYorkSandyPTSD;

	HouseHoldMap _newYorkHHmap;
	IncomeMap _newYorkIncMap;
	SubIncomeMap _newYorkSubIncMap;
	ExpenseMap _newYorkExpenseMap;
	SandyDamageMap _newYorkSandyDamageMap;
	SandyFinLossMap _newYorkSandyFinLossMap;
	SandyPTSDMap _newYorkSandyPTSDMap;
};
#endif __FileHandling_h__