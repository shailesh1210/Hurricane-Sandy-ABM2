#include "FileHandling.h"
#include "Parameters.h"
#include <string>


FileHandling::FileHandling()
{

}

FileHandling::~FileHandling()
{
	std::cout << "File Destructor!" << std::endl;
}

void FileHandling::readInputFile()
{
	readDemoFileNYC();
	readHHsizeFileNYC();
	readIncomeFileNYC();
	readSubIncomeFileNYC();
	
}

void FileHandling::readSandyDamageFile()
{
	readSandyDamagePredictorFileNYC();
	readSandyFinancialLossFileNYC();
	readSandyPTSDFileNYC();
}

void FileHandling::readExpenseFile()
{
	std::string file = "input\\spending\\spendingRatio.txt";
	std::ifstream ifs(file.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		Expense nyc;
		ifs >> nyc.inHHsize;
		ifs >> nyc.minIncome;
		ifs >> nyc.maxIncome;
		ifs >> nyc.ratioExpenseInc;

		//_newYorkExpense.push_back(nyc);
		_newYorkExpenseMap.insert(std::pair<int, Expense>(nyc.inHHsize, nyc));
	}
}

void FileHandling::readDemoFileNYC()
{
	std::string file1 = "input\\demo\\pop_NYC.txt";
	std::ifstream ifs(file1.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		Demographics nyc;
		ifs >> nyc.pDemoAtt;
		ifs >> nyc.inBorough;
		ifs >> nyc.inRace;
		ifs >> nyc.inGender;
		ifs >> nyc.inAge;

		_newYork.push_back(nyc);
	}
}

void FileHandling::readHHsizeFileNYC()
{
	std::string file2 = "input\\household\\household_NYC.txt";
	std::ifstream ifs(file2.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		Household nyc;
		ifs >> nyc.pHHsize;
		ifs >> nyc.inHHSize;
		ifs >> nyc.inBoro;

		//_newYorkHHsize.push_back(nyc);
		_newYorkHHmap.insert(std::pair<int, Household> (nyc.inBoro, nyc));
	}
}

void FileHandling::readIncomeFileNYC()
{
	std::string file3 = "input\\income\\income_NYC.txt";
	std::ifstream ifs(file3.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		Income nyc;
		ifs >> nyc.pIncome;
		ifs >> nyc.inHHsize;
		ifs >> nyc.inBoro;
		ifs >> nyc.inIncome;

		//_newYorkIncome.push_back(nyc);

		int incKey = 10*nyc.inHHsize + nyc.inBoro;
		_newYorkIncMap.insert(std::pair<int, Income>(incKey, nyc));
	}
}

void FileHandling::readSubIncomeFileNYC()
{
	std::string file4 = "input\\subIncome\\subincome_NYC.txt";
	std::ifstream ifs(file4.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		SubIncome nyc;
		ifs >> nyc.pSubInc;
		ifs >> nyc.inIncome;
		ifs >> nyc.inSubIncome;
		ifs >> nyc.inBoro;
		ifs >> nyc.minIncome;
		ifs >> nyc.maxIncome;

		//_newYorkIncDiv.push_back(nyc);

		int subIncKey = 10*nyc.inIncome + nyc.inBoro;
		_newYorkSubIncMap.insert(std::pair<int, SubIncome>(subIncKey, nyc));
	}
}

void FileHandling::readSandyDamagePredictorFileNYC()
{
	std::string file5 = "input\\sandyDamage\\sandy_damage_nyc.txt";
	std::ifstream ifs(file5.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		SandyDamage nyc;
		ifs >> nyc.pSandyDamage;
		ifs >> nyc.inDamage;
		ifs >> nyc.inIncome;
		ifs >> nyc.inBoro;

		//_newYorkSandyDamage.push_back(nyc);

		int damageKey = 10*nyc.inIncome + nyc.inBoro;
		_newYorkSandyDamageMap.insert(std::pair<int, SandyDamage>(damageKey, nyc));
	}
}

void FileHandling::readSandyFinancialLossFileNYC()
{
	std::string file7 = "input\\sandyFinancialLoss\\sandyFinancialLossType.txt";
	std::ifstream ifs(file7.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		SandyFinancialLoss nyc;
		ifs >> nyc.pFinLossType;
		ifs >> nyc.inBorough;
		ifs >> nyc.inFinLossType;
		ifs >> nyc.minLossAmnt;
		ifs >> nyc.maxLossAmnt;

		//_newYorkSandyFinLoss.push_back(nyc);
		_newYorkSandyFinLossMap.insert(std::pair<int, SandyFinancialLoss>(nyc.inBorough, nyc));
	}
}

void FileHandling::readSandyPTSDFileNYC()
{
	std::string file6 = "input\\PTSD\\sandyPTSD.txt";
	std::ifstream ifs(file6.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		SandyPTSD nyc;
		ifs >> nyc.pSandyPTSD;
		ifs >> nyc.inIncomeLoss;
		ifs >> nyc.inLeftHome;
		ifs >> nyc.inPTSDstatus;
		ifs >> nyc.inPTSDx;

		//_newYorkSandyPTSD.push_back(nyc);
		int keyPTSD = 10*nyc.inIncomeLoss + nyc.inLeftHome;
		_newYorkSandyPTSDMap.insert(std::pair<int, SandyPTSD>(keyPTSD, nyc));
	}

}






