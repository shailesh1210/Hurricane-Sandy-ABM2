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
	readBoroughFileNYC();
	readDemoFileNYC();
	readHHsizeFileNYC();
	readIncomeFileNYC();
	readSubIncomeFileNYC();

	readExpenseFile();
	readSandyDamageFile();
	readSandyFinancialReimbursementFileNYC();
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
		newYorkExpenseMap.insert(std::pair<unsigned short int, Expense>(nyc.inHHsize, nyc));
	}
}

void FileHandling::readBoroughFileNYC()
{
	std::string file = "input\\demo\\borough_NYC.txt";
	std::ifstream ifs(file.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		Borough nyc;
		ifs >> nyc.pBorough;
		ifs >> nyc.inBorough;

		newYorkBoro.push_back(nyc);
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

		//newYork.push_back(nyc);
		newYorkMap.insert(std::pair<unsigned short int, Demographics>(nyc.inBorough, nyc));
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
		newYorkHHmap.insert(std::pair<unsigned short int, Household> (nyc.inBoro, nyc));
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

		unsigned short int incKey = 10*nyc.inHHsize + nyc.inBoro;
		newYorkIncMap.insert(std::pair<unsigned short int, Income>(incKey, nyc));
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

		unsigned short int subIncKey = 10*nyc.inIncome + nyc.inBoro;
		newYorkSubIncMap.insert(std::pair<unsigned short int, SubIncome>(subIncKey, nyc));
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

		unsigned short int damageKey = 10*nyc.inIncome + nyc.inBoro;
		newYorkSandyDamageMap.insert(std::pair<unsigned short int, SandyDamage>(damageKey, nyc));
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
		newYorkSandyFinLossMap.insert(std::pair<unsigned short int, SandyFinancialLoss>(nyc.inBorough, nyc));
	}
}

void FileHandling::readSandyPTSDFileNYC()
{
	std::string file6 = "input\\PTSD\\sandyPTSD.txt";
	std::ifstream ifs(file6.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		SandyPTSDstatus nyc;
		ifs >> nyc.pSandyPTSD;
		ifs >> nyc.inIncomeLoss;
		ifs >> nyc.inGender;
		ifs >> nyc.inAgeCat;
		//ifs >> nyc.inLeftHome;
		ifs >> nyc.inPTSDstatus;

		//_newYorkSandyPTSD.push_back(nyc);
		//unsigned short int keyPTSD = 10*nyc.inIncomeLoss + nyc.inLeftHome;
		unsigned short int keyPTSD = 100*nyc.inIncomeLoss + 10*nyc.inGender + nyc.inAgeCat;
		newYorkSandyPTSDMap.insert(std::pair<unsigned short int, SandyPTSDstatus>(keyPTSD, nyc));
	}

	std::string file7 = "input\\PTSD\\sandyPTSDsymp.txt";
	std::ifstream ifs1(file7.c_str(), std::ios::in);

	while(!ifs1.eof())
	{
		SandyPTSDx nyc;
		ifs1 >> nyc.keyPTSDx;
		ifs1 >> nyc.inPTSDx;

		newYorkSandyPTSDsympMap.insert(std::pair<unsigned short int, float>(nyc.keyPTSDx, nyc.inPTSDx));
	}

}

void FileHandling::readSandyFinancialReimbursementFileNYC()
{
	std::string file8 = "input\\sandyFinancialReimbursement\\sandyReimbursement.txt";
	std::ifstream ifs(file8.c_str(), std::ios::in);

	while(!ifs.eof())
	{
		SandyFinancialReimbursement nyc;
		ifs >> nyc.pReimbursement;
		ifs >> nyc.inBorough;

		newYorkSandyFinReimburseMap.insert(std::pair<unsigned short int, float>(nyc.inBorough, nyc.pReimbursement));
	}
}






