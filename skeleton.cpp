#include "skeleton.h"
#include "FileHandling.h"
#include "Agent.h"
#include "Family.h"
#include "Statistics.h"

#include <fstream>
#include <ctime>
#include <boost\range\algorithm.hpp>
#include <QVBoxLayout>
//#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextBrowser>
#include <QMap>
#include <QDebug>


Skeleton::Skeleton(QWidget *parent)
	: QWidget(parent)
{
	/*QGridLayout *grid = new QGridLayout(this);
	grid->addWidget(createFirstGroupBox(), 0, 0);
	grid->addWidget(createSecondGroupBox(), 1, 0);
	grid->addWidget(createThirdGroupBox(), 2, 0);
	grid->addWidget(createFourthGroupBox(), 0, 1);*/

	QVBoxLayout *vboxLeft = new QVBoxLayout;
	vboxLeft->addWidget(createFirstGroupBox());
	vboxLeft->addWidget(createSecondGroupBox());

	QVBoxLayout *vboxRight = new QVBoxLayout;
	vboxRight->addWidget(createFourthGroupBox());
	vboxRight->addWidget(createThirdGroupBox());

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addLayout(vboxLeft);
	mainLayout->addLayout(vboxRight);

	////ui.setupUi(this);

	connect(_initialize, SIGNAL(clicked()), this, SLOT(initialize()));
	connect(_run, SIGNAL(clicked()), this, SLOT(run()));
	connect(_reset, SIGNAL(clicked()), this, SLOT(reset()));
	connect(_showStats, SIGNAL(clicked()), this, SLOT(stats()));
	connect(steppedCareCB, SIGNAL(stateChanged(int)), this, SLOT(enableGroupBoxSC(int)));
	connect(usualCareCB, SIGNAL(stateChanged(int)), this, SLOT(enableGroupBoxUC(int)));

}

Skeleton::~Skeleton()
{
	
}

QGroupBox *Skeleton::createFirstGroupBox()
{
	inputParamGrpBox = new QGroupBox(tr("Input Parameters"));

	numAgentsInput = new QLineEdit("100000", this);
	numStepsInput = new QLineEdit("730", this);
	//numTrialsInput = new QLineEdit("50", this);

	numBoroInput = new QLineEdit(tr("5"), this);
	numAgeCatInput = new QLineEdit(tr("3"), this);
	numRaceCatInput = new QLineEdit(tr("4"), this);
	numIncCatInput = new QLineEdit(tr("3"), this);
	numSubIncCatInput = new QLineEdit(tr("7"), this);

	numBoroInput->setReadOnly(true);
	numAgeCatInput->setReadOnly(true);
	numRaceCatInput->setReadOnly(true);
	numIncCatInput->setReadOnly(true);
	numSubIncCatInput->setReadOnly(true);

	numAgentsInput->setValidator(new QIntValidator(0,10000000,this));
	numStepsInput->setValidator(new QIntValidator(0,10000000,this));
	//numTrialsInput->setValidator(new QIntValidator(0,10000000,this));
	
	QFormLayout *formLayout = new QFormLayout;

	formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	formLayout->addRow("Agent Population  ", numAgentsInput);
	formLayout->addRow("Simulation Time(Days)  ", numStepsInput);
	//formLayout->addRow("Number of trials  ", numTrialsInput);

	formLayout->addRow("Number of Boroughs", numBoroInput);
	formLayout->addRow("Age Categories ", numAgeCatInput);
	formLayout->addRow("Race Categories ", numRaceCatInput);
	formLayout->addRow("Income Categories ", numIncCatInput);
	formLayout->addRow("Sub-Income Categories ", numSubIncCatInput);

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(formLayout);

	inputParamGrpBox->setLayout(vbox);

	return inputParamGrpBox;
}

QGroupBox *Skeleton::createSecondGroupBox()
{
	QStringList costSteppedCare;
	QStringList costUsualCare;

	QStringList sessionsCBT;
	QStringList sessionsSPR;

	costSteppedCare << "60" << "120";
	costUsualCare << "15" << "30";

	sessionsCBT << "8" << "12";
	sessionsSPR << "5" << "8";

	trtmentGrpBox = new QGroupBox(tr("Select treatment Type"));
	steppedCareCB = new QCheckBox(tr("Stepped Care"));
	usualCareCB = new QCheckBox(tr("Usual Care"));

	costComboCBT = new QComboBox;
	costComboSPR = new QComboBox;
	costComboSPR1 = new QComboBox;

	sessionComboCBT = new QComboBox;
	sessionComboSPR = new QComboBox;
	sessionComboSPR1 = new QComboBox;

	costGrpBoxSC = createTrtmentSubGroupBox(trtmentGrpBox, costComboCBT, costComboSPR, "Cost", "CBT cost/session($)", costSteppedCare, "SPR cost/session", costUsualCare);
	sessionGrpBoxSC = createTrtmentSubGroupBox(trtmentGrpBox, sessionComboCBT, sessionComboSPR, "Session", "CBT sessions", sessionsCBT, "SPR sessions", sessionsSPR);

	costGrpBoxUC = createTrtmentSubGroupBox(trtmentGrpBox, costComboSPR1, NULL,  "Cost", "SPR cost/session($)", costUsualCare, " ", QStringList());
	sessionGrpBoxUC = createTrtmentSubGroupBox(trtmentGrpBox, sessionComboSPR1, NULL,  "Session", "SPR sessions", sessionsSPR , " ", QStringList());

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(steppedCareCB);
	vbox->addWidget(costGrpBoxSC);
	vbox->addWidget(sessionGrpBoxSC);

	vbox->addWidget(usualCareCB);
	vbox->addWidget(costGrpBoxUC);
	vbox->addWidget(sessionGrpBoxUC);

	trtmentGrpBox->setLayout(vbox);

	return trtmentGrpBox;
}

QGroupBox *Skeleton::createThirdGroupBox()
{
	pushBtnGrpBox = new QGroupBox(tr("Press any button"));

	_initialize = new QPushButton("Intialize", this);
	_run = new QPushButton("Run", this);
	_reset = new QPushButton("Reset", this);
	_showStats = new QPushButton("Show Stats", this);

	_run->setDisabled(true);
	_showStats->setDisabled(true);

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(_initialize);
	vbox->addWidget(_run);
	vbox->addWidget(_reset);
	vbox->addWidget(_showStats);

	pushBtnGrpBox->setLayout(vbox);

	return pushBtnGrpBox;
}

QGroupBox *Skeleton::createFourthGroupBox()
{
	resourcesGrpBox = new QGroupBox(tr("Resources"));

	mealPerDay = new QLineEdit(tr("2"), this);
	mealPerDay->setValidator(new QIntValidator(0, 4));

	durMealDist = new QLineEdit(tr("100"), this);
	durMealDist->setValidator(new QIntValidator(0, 730));

	bronxMealServed = new QLineEdit(tr("229000"), this);
	bronxMealServed->setValidator(new QIntValidator(0, 10000000, this));
	bronxGrpBox = createResourcesSubGroupBox(resourcesGrpBox, bronxMealServed,"Bronx");

	brookMealServed = new QLineEdit(tr("2700000"), this);
	brookMealServed->setValidator(new QIntValidator(0, 10000000, this));
	brookGrpBox = createResourcesSubGroupBox(resourcesGrpBox, brookMealServed, "Brooklyn");

	manhMealServed = new QLineEdit(tr("577000"), this);
	manhMealServed->setValidator(new QIntValidator(0, 10000000, this));
	manhGrpBox = createResourcesSubGroupBox(resourcesGrpBox, manhMealServed, "Manhattan");

	queensMealServed = new QLineEdit(tr("1500000"), this);
	queensMealServed->setValidator(new QIntValidator(0, 10000000, this));
	queensGrpBox = createResourcesSubGroupBox(resourcesGrpBox, queensMealServed, "Queens");

	statenMealServed = new QLineEdit(tr("397000"), this);
	statenMealServed->setValidator(new QIntValidator(0, 10000000, this));
	statenGrpBox = createResourcesSubGroupBox(resourcesGrpBox, statenMealServed, "Staten Island");

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	formLayout->addRow("Number of meals/day", mealPerDay);
	formLayout->addRow("Duration of meal distribution (days)", durMealDist);
	formLayout->addRow(bronxGrpBox);
	formLayout->addRow(brookGrpBox);
	formLayout->addRow(manhGrpBox);
	formLayout->addRow(queensGrpBox);
	formLayout->addRow(statenGrpBox);

	resourcesGrpBox->setLayout(formLayout);

	return resourcesGrpBox;
}

QGroupBox *Skeleton::createTrtmentSubGroupBox(QGroupBox *grpBox, QComboBox *combo1, QComboBox *combo2, const QString boxTitle, const QString c1Label, QStringList c1Items , const QString c2Label, QStringList c2Items)
{
	QGroupBox *subGrpBox = new QGroupBox(boxTitle, grpBox);
	subGrpBox->setDisabled(true);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

	//QComboBox *combo1 = new QComboBox;
	combo1->addItems(c1Items);
	formLayout->addRow(c1Label, combo1);

	//QComboBox *combo2 = new QComboBox;
	if(!c2Items.isEmpty() && combo2 != NULL)
	{
		combo2->addItems(c2Items);
		formLayout->addRow(c2Label, combo2);
	}
	
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addLayout(formLayout);

	subGrpBox->setLayout(vbox);

	return subGrpBox;
	
}

QGroupBox *Skeleton::createResourcesSubGroupBox(QGroupBox *grpBox, QLineEdit *mealServed, const QString boxTitle)
{
	QGroupBox *subGrpBox = new QGroupBox(boxTitle, grpBox);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	formLayout->addRow("Meals served", mealServed);

	subGrpBox->setLayout(formLayout);

	return subGrpBox;
}

void Skeleton::initialize()
{

	if((numAgentsInput->text() == "0" || numAgentsInput->text().isEmpty()) ||(numStepsInput->text() == "0" || numStepsInput->text().isEmpty()))
	{
		QMessageBox::information(this, tr("Empty Field"), tr("Check input parameters. Please enter a valid number!"));
		return;
	}
	else
	{
		clock_t t;
		t = clock();
		std::cout << "Calculating... " << std::endl;

		_agentListFinal.clear();
		_numAgents = numAgentsInput->text().toInt();
		_numSteps = numStepsInput->text().toInt();
		//_numTrials = inNumTrials;

		_numBoro = numBoroInput->text().toInt();
		_ageCat = numAgeCatInput->text().toInt();
		_raceCat = numRaceCatInput->text().toInt();
		_incCat = numIncCatInput->text().toInt();
		_subIncCat = numSubIncCatInput->text().toInt();

		_mealInventoryBronx = bronxMealServed->text().toInt();
		_mealInventoryBrook = brookMealServed->text().toInt();
		_mealInventoryManh = manhMealServed->text().toInt();
		_mealInventoryQueens = queensMealServed->text().toInt();
		_mealInventoryStaten = statenMealServed->text().toInt();

		_numPplServedBronx = _mealInventoryBronx / (mealPerDay->text().toInt() * durMealDist->text().toInt());
		_numPplServedBrook = _mealInventoryBrook / (mealPerDay->text().toInt() * durMealDist->text().toInt());
		_numPplServedManh = _mealInventoryManh / (mealPerDay->text().toInt() * durMealDist->text().toInt());
		_numPplServedQueens = _mealInventoryQueens / (mealPerDay->text().toInt() * durMealDist->text().toInt());
		_numPplServedStaten = _mealInventoryStaten / (mealPerDay->text().toInt() * durMealDist->text().toInt());

		if(steppedCareCB->isChecked())
		{
			_steppedCare = true;
			_costCBTtrtment = costComboCBT->currentText().toInt();
			_costSPRtrtment = costComboSPR->currentText().toInt();
			_sessionCBT = sessionComboCBT->currentText().toInt();
			_sessionSPR = sessionComboSPR->currentText().toInt();
		}
		else
		{
			_steppedCare = false;
			_costCBTtrtment = 0;
			_costSPRtrtment = 0;
			_sessionCBT = 0;
			_sessionSPR = 0;
		}

		if(usualCareCB->isChecked())
		{
			_usualCare = true;
			_costSPR1trtment = costComboSPR1->currentText().toInt();
			_sessionSPR1 = sessionComboSPR1->currentText().toInt();
		}
		else
		{
			_usualCare = false;
			_costSPR1trtment = 0;
			_sessionSPR1 = 0;
		}
		
		createAgents();
		createHousehold();

		t = clock() - t;
		std::cout << std::endl;
		std::cout << "Calculation Time:" << ((float)t / CLOCKS_PER_SEC) << "seconds" << std::endl;
		std::cout << std::endl;

		QMessageBox::information(this, tr("Success!"), tr("Agent's variables successfully initalized!"));
		
		_showStats->setDisabled(false);
		_run->setDisabled(false);
	

		//agentStats();

	}

}

void Skeleton::run()
{
	timeCounter = 0;

	customPlot = new QCustomPlot;
	customPlot->resize(1000, 400);
	customPlot->show();

	customPlot->addGraph(); // blue line
	customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
	customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross,5));
	customPlot->xAxis->setRange(0, _numSteps+10);
	customPlot->yAxis->setRange(0, 7);

	
	connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
	dataTimer.start(0);


	/*for(int j = 0; j < _numSteps; j++)
	{
		std::cout << "Iteration number: " << j << "\n\n";
		for(auto it1 = _famList.begin(); it1 != _famList.end(); ++it1)
		{
			for(auto it2 = it1->_agentFamList.begin(); it2 != it1->_agentFamList.end(); ++it2)
			{
				std::cout << it2->getAgeCat() << "\t" << it2->getRace() << "\t" << it2->getIncome() << "\t" << it2->getBorough() << "\t" << it2->getHHsize() << std::endl;
			}

			std::cout << std::endl;
		}

		std::cout << "\n";
	}*/

	_run->setDisabled(true);
}

void Skeleton::reset()
{
	numAgentsInput->setText("100000");
	numStepsInput->setText("100");
	//numTrialsInput->setText("50");

	_showStats->setDisabled(true);
	_run->setDisabled(true);
}

void Skeleton::stats()
{
	QWidget *demoWidget = new QWidget(this);
	demoWidget->setWindowFlags(Qt::Window); //creates another widget in separate window http://doc.qt.io/qt-5/qtwidgets-widgets-windowflags-example.html

	QVBoxLayout *vbox = new QVBoxLayout(demoWidget);
	QTextBrowser *statsViewer = new QTextBrowser(demoWidget);

	Statistics *_stats = new Statistics(this, _agentListFinal, _numBoro, _ageCat, _raceCat, _incCat, _numAgents);
	_stats->demographicStats();
	QString outString = _stats->outString;
	statsViewer->setPlainText(outString);

	vbox->addWidget(statsViewer);
	demoWidget->setWindowTitle(tr("Demographic Stats NYC"));
	demoWidget->resize(680,300);
	demoWidget->show();

	_showStats->setDisabled(true);

}

void Skeleton::enableGroupBoxSC(int state)
{
	if(state){
		costGrpBoxSC->setDisabled(false);
		sessionGrpBoxSC->setDisabled(false);
	}
	else{
		costGrpBoxSC->setDisabled(true);
		sessionGrpBoxSC->setDisabled(true);
	}
}

void Skeleton::enableGroupBoxUC(int state)
{
	if(state){
		costGrpBoxUC->setDisabled(false);
		sessionGrpBoxUC->setDisabled(false);	
	}
	else{
		costGrpBoxUC->setDisabled(true);
		sessionGrpBoxUC->setDisabled(true);
	}
}

void Skeleton::realtimeDataSlot()
{
	int countPTSD = 0;

	if(timeCounter < _numSteps)
	{
		boost::range::random_shuffle(_agentListFinal);
		for(auto agent = _agentListFinal.begin(); agent != _agentListFinal.end(); ++agent)
		{
			agent->decayPTSDsymptom();
			float PTSDx = agent->getPTSDsymptom();
			if(PTSDx > 6.0)
			{
				countPTSD++;
			}
		}

		float perPTSD = (float)countPTSD/_numAgents;
		std::cout << timeCounter << "\t" << perPTSD << "\t" << _agentListFinal.size() << std::endl;

		customPlot->graph(0)->addData(timeCounter, 100*perPTSD);
	}
	else
	{
		dataTimer.stop();
	}

	customPlot->replot();
	timeCounter++;
}


void Skeleton::createAgents()
{
	FileHandling *file = new FileHandling();
	file->readInputFile();

	_famList.clear();
	int famIdx = 0;

	for(int i = 0; i < _numAgents; i++)
	{
		Agent *myAgent = new Agent(file);
		myAgent->setAgentAttributes(i);

		if(myAgent->getHHsize() == HHsize1){createHHSize1(myAgent, famIdx);}  //add agents with HHsize 1 to the familyList}
		else{_tempAgentsMap.insert(std::pair<int, Agent> (myAgent->getKey(), *myAgent));} //creates hashtable for HHsize 2 or more 
	}

}

void Skeleton::createHousehold()
{
	_famListHHsize2.clear();

	int famIdx = _famList.size()+1;

	createHHSize234Step1(famIdx); 
	createHHSize234Step2(famIdx); 
	mergeHouseholds();

	FileHandling *file = new FileHandling();

	file->readExpenseFile();
	file->readSandyDamageFile();

	for(auto f = _famList.begin(); f != _famList.end(); ++f)
	{
		f->calcAverageFamilyIncome();
		f->calcAverageFamilySaving(file);
		f->predictSandyDamage(file);

		for(auto agent = f->_agentFamList.begin(); agent != f->_agentFamList.end(); ++agent)
		{
			_agentListFinal.push_back(*agent);
		}
	}

}


void Skeleton::mergeHouseholds()
{
	for(auto f1 = _famListHHsize2.begin(); f1 != _famListHHsize2.end(); ++f1)
	{
		_famList.push_back(*f1);
	}
}


void Skeleton::createHHSize1(Agent *agent, int &index)
{
	index++;
	Family hhSize1(index);
	hhSize1.setFamilyAttributes(*agent);
	hhSize1.addAgent(*agent);
	_famList.push_back(hhSize1);

}

void Skeleton::createHHSize234Step1(int &index)
{
	for(auto agent1 = _tempAgentsMap.begin(); agent1 != _tempAgentsMap.end();)
	{
		Family hhSize234(index++);
		hhSize234.setFamilyAttributes(agent1->second);
		hhSize234.addAgent(agent1->second);

		auto its = _tempAgentsMap.equal_range(agent1->first);
		for(auto agent2 = its.first; agent2 != its.second;)
		{
			if(agent1->second.getID() != agent2->second.getID())
			{
				if(agent1->second.getRace() == agent2->second.getRace() && hhSize234._agentFamList.size() < agent1->second.getHHsize())
				{
					hhSize234.addAgent(agent2->second);
					agent2 = _tempAgentsMap.erase(agent2);
					if(hhSize234._agentFamList.size() == agent1->second.getHHsize()){break;}
				}
				else{++agent2;}
			}
			else{++agent2;}
		}

		if(hhSize234._agentFamList.size() > HHsize1 && hhSize234._agentFamList.size() <= agent1->second.getHHsize())
		{
			_famListHHsize2.push_back(hhSize234);
			agent1 = _tempAgentsMap.erase(agent1);
		}
		else{++agent1;}
	}

	fillIncompleteHouseholds(); //fill the households with remaining unmatched agents if size of family list is less than household size
}


void Skeleton::createHHSize234Step2(int &index)
{
	for(auto agent1 = _tempAgentsMap.begin(); agent1 != _tempAgentsMap.end();)
	{
		Family hhSize234(index++);
		hhSize234.setFamilyAttributes(agent1->second);
		hhSize234.addAgent(agent1->second);

		auto its = _tempAgentsMap.equal_range(agent1->first);
		for(auto agent2 = its.first; agent2 != its.second;)
		{
			if(agent1->second.getID() != agent2->second.getID())
			{
				if(hhSize234._agentFamList.size() < agent1->second.getHHsize())
				{
					hhSize234.addAgent(agent2->second);
					agent2 = _tempAgentsMap.erase(agent2);
					if(hhSize234._agentFamList.size() == agent1->second.getHHsize()){break;}
				}
				else{++agent2;}
			}
			else{++agent2;}
		}

		if(hhSize234._agentFamList.size() > HHsize1 && hhSize234._agentFamList.size() <= agent1->second.getHHsize())
		{
			_famListHHsize2.push_back(hhSize234);
			agent1 = _tempAgentsMap.erase(agent1);
		}
		else{++agent1;}
	}

	addUnmatchedAgents(index); //Add unmatched agents to the family list
}


void Skeleton::fillIncompleteHouseholds()
{
	for(auto f1 = _famListHHsize2.begin(); f1 !=_famListHHsize2.end();++f1)
	{
		if((f1->getFamilySize() != f1->_agentFamList.size() || f1->getFamilySize() == HHsize4) && f1->getFamilySize() > HHsize2)
		{
			auto its = _tempAgentsMap.equal_range(f1->getFamilyKey());
			for(auto agent = its.first; agent != its.second;)
			{
				if(f1->getFamilySize() == HHsize3 && f1->_agentFamList.size() < HHsize3)
				{
					f1->addAgent(agent->second);
					agent = _tempAgentsMap.erase(agent);
				}
				else if(f1->getFamilySize() == HHsize4 && f1->_agentFamList.size() < MAX_PEOPLE_HHSIZE4)
				{
					f1->addAgent(agent->second);
					agent = _tempAgentsMap.erase(agent);
				}
				else{++agent;}
			}
		}
	}
}

void Skeleton::addUnmatchedAgents(int &index)
{
	for(auto agent = _tempAgentsMap.begin(); agent !=_tempAgentsMap.end(); ++agent)
	{
		index++;

		Family remFamily(index);
		remFamily.setFamilyAttributes(agent->second);
		remFamily.addAgent(agent->second);
		_famListHHsize2.push_back(remFamily);
	}

	_tempAgentsMap.clear();
}







