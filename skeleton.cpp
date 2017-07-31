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

Skeleton::Skeleton(QWidget *parent) : QWidget(parent) 
{
	QVBoxLayout *vBoxLeft = new QVBoxLayout;

	vBoxLeft->addWidget(createFirstGroupBox());
	vBoxLeft->addWidget(createSecondGroupBox());
	vBoxLeft->addWidget(createThirdGroupBox());
	vBoxLeft->addWidget(createFourthGroupBox());
	
	red = QVector <int>() << 255 << 0 << 204 << 0 << 153 << 255 << 128 << 0;
	blue = QVector<int>() << 0 << 102 << 102 << 153 << 0 << 0 << 128 << 0;
	green = QVector<int>() << 0 << 204 << 0 << 76 << 76 << 127 << 128 << 0;

	QMap <int, QString> ptsdPlotTitle;
	//ptsdPlotTitle.insert(0, "PTSD persistence and treatment uptake");
	ptsdPlotTitle.insert(0, "Income Loss = False");
	ptsdPlotTitle.insert(1, "Income Loss = True");

	for(unsigned int i = 0 ; i < NUM_PTSD_PLOTS; i++)
	{
		ptsdPlot[i] = createPTSDPlot(ptsdPlotTitle[i]);
	}

	statePlot1 = createStatePlot("State transition (Stepped Care with Social Services)");
	//statePlot1 = createStatePlot("State transition (Stepped Care without Social Services)");
	statePlot2 = createStatePlot("State transition (Usual Care)");

	QVBoxLayout *vBoxMiddle = new QVBoxLayout;
	vBoxMiddle->addWidget(ptsdPlot[0]);
	//vBoxMiddle->addWidget(ptsdPlot[2]);
	vBoxMiddle->addWidget(statePlot1);

	QVBoxLayout *vBoxRight = new QVBoxLayout;
	vBoxRight->addWidget(ptsdPlot[1]);
	//vBoxRight->addWidget(ptsdPlot[3]);
	vBoxRight->addWidget(statePlot2);

	QHBoxLayout *hBoxRight = new QHBoxLayout;
	hBoxRight->addLayout(vBoxMiddle);
	hBoxRight->addLayout(vBoxRight);

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->addLayout(vBoxLeft);
	mainLayout->addLayout(hBoxRight,1);

	connect(initializePb, SIGNAL(clicked()), this, SLOT(initialize()));
	connect(runPb, SIGNAL(clicked()), this, SLOT(run()));
	connect(resetPb, SIGNAL(clicked()), this, SLOT(reset()));
	connect(showStatsPb, SIGNAL(clicked()), this, SLOT(stats()));
	connect(steppedCareCB, SIGNAL(clicked()), this, SLOT(enableSubGroupBoxTrtment()));
	connect(usualCareCB, SIGNAL(clicked()), this, SLOT(enableSubGroupBoxTrtment()));
}

Skeleton::~Skeleton()
{
	std::cout << "World destructor called!" << std::endl;
}

QGroupBox *Skeleton::createFirstGroupBox()
{
	inputParamGrpBox = new QGroupBox(tr("Input Parameters"));

	numAgentsInput = new QLineEdit(tr("1500000"), inputParamGrpBox);
	numStepsInput = new QLineEdit(tr("104"), inputParamGrpBox);
	//numTrialsInput = new QLineEdit("50", this);

	numBoroInput = new QLineEdit(tr("5"), inputParamGrpBox);
	numAgeCatInput = new QLineEdit(tr("3"), inputParamGrpBox);
	numRaceCatInput = new QLineEdit(tr("4"), inputParamGrpBox);
	numIncCatInput = new QLineEdit(tr("3"), inputParamGrpBox);
	numSubIncCatInput = new QLineEdit(tr("7"), inputParamGrpBox);

	numBoroInput->setReadOnly(true);
	numAgeCatInput->setReadOnly(true);
	numRaceCatInput->setReadOnly(true);
	numIncCatInput->setReadOnly(true);
	numSubIncCatInput->setReadOnly(true);

	numAgentsInput->setValidator(new QIntValidator(0,10000000,inputParamGrpBox));
	numStepsInput->setValidator(new QIntValidator(0,10000000,inputParamGrpBox));
	//numTrialsInput->setValidator(new QIntValidator(0,10000000,this));
	
	QFormLayout *formLayout = new QFormLayout;

	//formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
	formLayout->addRow("Agent Population  ", numAgentsInput);
	formLayout->addRow("Simulation Time (Weeks)  ", numStepsInput);
	
	//formLayout->addRow("Number of trials  ", numTrialsInput);
	formLayout->addRow("Number of Boroughs", numBoroInput);
	formLayout->addRow("Age Categories ", numAgeCatInput);
	formLayout->addRow("Race Categories ", numRaceCatInput);
	formLayout->addRow("Income Categories ", numIncCatInput);
	formLayout->addRow("Sub-Income Categories ", numSubIncCatInput);

	inputParamGrpBox->setLayout(formLayout);
	return inputParamGrpBox;
}

QGroupBox *Skeleton::createSecondGroupBox()
{
	trtmentGrpBox = new QGroupBox(tr("Treatment Parameters"));

	trtmentTypeGrpBox = createTrtmentSubGroupBox(trtmentGrpBox, "Select Treatment Type", 0);
	screeningGrpBox = createTrtmentSubGroupBox(trtmentGrpBox, "Screening", 1);
	trtmentTimeGrpBox = createTrtmentSubGroupBox(trtmentGrpBox, "Treatment Duration", 2);
	trtmentStrengthGrpBox = createTrtmentSubGroupBox(trtmentGrpBox, "Strength", 3);
	costGrpBox = createTrtmentSubGroupBox(trtmentGrpBox, "Cost", 4);
	sessionGrpBox = createTrtmentSubGroupBox(trtmentGrpBox, "Session", 5);

	QFormLayout *formLayout = new QFormLayout;

	formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
	formLayout->addRow(trtmentTypeGrpBox);
	formLayout->addRow(screeningGrpBox);
	formLayout->addRow(trtmentTimeGrpBox);
	formLayout->addRow(trtmentStrengthGrpBox);
	formLayout->addRow(costGrpBox);
	formLayout->addRow(sessionGrpBox);

	trtmentGrpBox->setLayout(formLayout);

	return trtmentGrpBox;
}

QGroupBox *Skeleton::createThirdGroupBox()
{
	relapseGrpBox = new QGroupBox(tr("Relapse Parameters"));

	relapsePTSDxInput = new QLineEdit(tr("12.0"), relapseGrpBox);
	relapsePTSDxInput->setValidator(new QDoubleValidator(0.0, 16.0, 4, relapseGrpBox));

	relapseTimeInput = new QLineEdit(tr("13"), relapseGrpBox);
	relapseTimeInput->setValidator(new QIntValidator(0, 104, relapseGrpBox));

	numRelapseInput = new QLineEdit(tr("1"), relapseGrpBox);
	numRelapseInput->setValidator(new QIntValidator(0, 10, relapseGrpBox));

	//propRelapse = new QLineEdit(tr("0.15"), relapseGrpBox);
	//propRelapse->setValidator(new QDoubleValidator(0, 1, 4, relapseGrpBox));

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
	formLayout->addRow("Relapse PTSDx ", relapsePTSDxInput);
	formLayout->addRow("Relapse Time (Weeks) ", relapseTimeInput);
	formLayout->addRow("Number of Relapse ", numRelapseInput);
	//formLayout->addRow("Relapse Proportion ", propRelapse);

	relapseGrpBox->setLayout(formLayout);

	return relapseGrpBox;
}

QGroupBox *Skeleton::createFourthGroupBox()
{
	pushBtnGrpBox = new QGroupBox(tr("Press any button"));

	initializePb = new QPushButton("Intialize", pushBtnGrpBox);
	runPb = new QPushButton("Run", pushBtnGrpBox);
	resetPb = new QPushButton("Reset", pushBtnGrpBox);
	showStatsPb = new QPushButton("Show Stats", pushBtnGrpBox);

	runPb->setDisabled(true);
	showStatsPb->setDisabled(true);

	QHBoxLayout *hboxTop = new QHBoxLayout;
	hboxTop->addWidget(initializePb);
	hboxTop->addWidget(runPb);

	/*QHBoxLayout *hboxBottom = new QHBoxLayout;
	hboxBottom->addWidget(_reset);
	hboxBottom->addWidget(_showStats);*/

	//QHBoxLayout *hboxBottom = new QHBoxLayout;
	hboxTop->addWidget(resetPb);
	hboxTop->addWidget(showStatsPb);

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
	formLayout->addRow(hboxTop);
	//formLayout->addRow(hboxBottom);
	
	pushBtnGrpBox->setLayout(formLayout);

	return pushBtnGrpBox;	
}


QGroupBox *Skeleton::createTrtmentSubGroupBox(QGroupBox *grpBox, const QString boxTitle, const int flag)
{
	QGroupBox *subGrpBox = new QGroupBox(boxTitle, grpBox);
	
	if(flag == 0)
	{
		steppedCareCB = new QCheckBox(tr("Stepped Care"));
		steppedCareCB->setChecked(true);
		usualCareCB = new QCheckBox(tr("Usual Care"));
		usualCareCB->setChecked(true);

		QHBoxLayout *hbox = new QHBoxLayout;
		hbox->addWidget(steppedCareCB);
		hbox->addWidget(usualCareCB);

		subGrpBox->setLayout(hbox);
	}
	else if(flag == 1)
	{
		sensitivityInput = new QLineEdit(tr("0.8"), subGrpBox);
		sensitivityInput->setValidator(new QDoubleValidator(0, 1, 4, subGrpBox));

		specificityInput = new QLineEdit(tr("0.8"), subGrpBox);
		specificityInput->setValidator(new QDoubleValidator(0, 1, 4, subGrpBox));

		screenStartTimeInput = new QLineEdit(tr("4"), subGrpBox);
		screenStartTimeInput->setValidator(new QIntValidator(0, 60, subGrpBox));

		ptsdCutOffInput = new QLineEdit(tr("6.0"), subGrpBox);
		ptsdCutOffInput->setValidator(new QDoubleValidator(0, 16, 4, subGrpBox));

		QFormLayout *formLayout = new QFormLayout;
		formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
		formLayout->addRow("Sensitivity", sensitivityInput);
		formLayout->addRow("Specificity", specificityInput);
		formLayout->addRow("PTSD Screening Start Time (Weeks)" , screenStartTimeInput);
		formLayout->addRow("PTSD Cut-off Threshold", ptsdCutOffInput);

		subGrpBox->setLayout(formLayout);
	}
	else if(flag == 2)
	{
		trtmentTimeInput = new QLineEdit(tr("104"), subGrpBox);
		trtmentTimeInput->setValidator(new QIntValidator(0, 2000, subGrpBox));

		trtmentWaitTimeInput = new QLineEdit(tr("1"), subGrpBox);
		trtmentWaitTimeInput->setValidator(new QIntValidator(0, 28, subGrpBox));

		socialWorkerTimeInput = new QLineEdit(tr("4"), subGrpBox);
		socialWorkerTimeInput->setValidator(new QIntValidator(0, 90, subGrpBox));

		QFormLayout *formLayout = new QFormLayout;

		formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);

		formLayout->addRow("Treatment Time (Weeks)", trtmentTimeInput);
		formLayout->addRow("Treatment Wait Time (Weeks)", trtmentWaitTimeInput);
		formLayout->addRow("Social Worker Service Time (Weeks)", socialWorkerTimeInput);

		subGrpBox->setLayout(formLayout);
	}
	else if(flag == 3)
	{
		cbtResolutionCoffInput = new QLineEdit(tr("0.36"), subGrpBox);
		cbtResolutionCoffInput->setValidator(new QDoubleValidator(0, 1, 4, subGrpBox));

		sprResolutionCoffInput = new QLineEdit(tr("0.2"), subGrpBox);
		sprResolutionCoffInput->setValidator(new QDoubleValidator(0, 1, 4, subGrpBox));

		naturalDecayCoffInput = new QLineEdit(tr("0.14"), subGrpBox);
		naturalDecayCoffInput->setValidator(new QDoubleValidator(0, 1, 4, subGrpBox));

		QFormLayout *formLayout = new QFormLayout;

		formLayout->setAlignment(Qt::AlignTop | Qt::AlignVCenter);
		formLayout->addRow("CBT Resolution Coefficient", cbtResolutionCoffInput);
		formLayout->addRow("SPR Resolution Coefficient", sprResolutionCoffInput);
		formLayout->addRow("Natural Decay Coefficient", naturalDecayCoffInput);

		subGrpBox->setLayout(formLayout);
	}
	else if(flag == 4)
	{
		QStringList costSteppedCare;
		QStringList costUsualCare;

		costSteppedCare << "60" << "120";
		costUsualCare << "15" << "30";

		costComboCBT = new QComboBox;
		costComboSPR = new QComboBox;

		costComboCBT->addItems(costSteppedCare);
		costComboSPR->addItems(costUsualCare);

		QFormLayout *formLayout = new QFormLayout;

		formLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		formLayout->addRow("CBT cost/session($)", costComboCBT);
		formLayout->addRow("SPR cost/session($)", costComboSPR);

		subGrpBox->setLayout(formLayout);

	}
	else if(flag == 5)
	{
		QStringList sessionsCBT;
		QStringList sessionsSPR;

		sessionsCBT << "8" << "12";
		sessionsSPR << "5" << "8";

		sessionComboCBT = new QComboBox;
		sessionComboSPR = new QComboBox;

		sessionComboCBT->addItems(sessionsCBT);
		sessionComboSPR->addItems(sessionsSPR);

		QFormLayout *formLayout = new QFormLayout;

		formLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		formLayout->addRow("CBT sessions", sessionComboCBT);
		formLayout->addRow("SPR sessions", sessionComboSPR);

		subGrpBox->setLayout(formLayout);
	}

	return subGrpBox;
}


QCustomPlot *Skeleton::createPTSDPlot(const QString titleName)
{
	QMap <int, QString> ptsdMap;
	ptsdMap.insert(0, "PTSD persistence(Stepped Care (SS))");
	ptsdMap.insert(1, "PTSD persistence(Stepped Care (w/o SS))");
	ptsdMap.insert(2, "PTSD persistence (Usual Care)");
	ptsdMap.insert(3, "Cognitive Behavior Therapy");
	ptsdMap.insert(4, "Skills for Psychological Recovery ");
	ptsdMap.insert(5, "Natural Decay ");

	QCustomPlot *ptsdTrtmentPlot = new QCustomPlot;
	ptsdTrtmentPlot->plotLayout()->insertRow(0);
	//QCPTextElement *title = new QCPTextElement(ptsdTrtmentPlot, "PTSD persistance and treatment", QFont("sans", 12, QFont::Bold));
	QCPTextElement *title = new QCPTextElement(ptsdTrtmentPlot, titleName, QFont("sans", 12, QFont::Bold));
	ptsdTrtmentPlot->plotLayout()->addElement(0, 0, title);

	for(int i = 0; i < NUM_PTSD_SUB_PLOTS; i++)
	{
		QPen solidPen;
		solidPen.setColor(QColor(red[i], blue[i], green[i]));
		solidPen.setWidth(3);

		ptsdTrtmentPlot->addGraph(); 
		ptsdTrtmentPlot->graph(i)->setPen(solidPen);
		ptsdTrtmentPlot->graph(i)->setName(ptsdMap[i]);
	}

	
	ptsdTrtmentPlot->xAxis->setRange(0, numStepsInput->text().toInt() + 10);
	ptsdTrtmentPlot->xAxis->grid()->setSubGridVisible(true);
	ptsdTrtmentPlot->xAxis->setLabel("Simulation Time(Weeks)");
	ptsdTrtmentPlot->xAxis->setLabelFont(QFont("sans", 10, QFont::Bold));

	ptsdTrtmentPlot->yAxis->setRange(0, 110);
	ptsdTrtmentPlot->yAxis->grid()->setSubGridVisible(true);
	ptsdTrtmentPlot->yAxis->setLabel("Percentage(%)");
	ptsdTrtmentPlot->yAxis->setLabelFont(QFont("sans", 10, QFont::Bold));
	

	ptsdTrtmentPlot->legend->setVisible(true);
	ptsdTrtmentPlot->legend->setBorderPen(QPen(QColor(255, 255, 255, 0.4)));
	ptsdTrtmentPlot->legend->setBrush(QColor(255, 255, 255, 0.4));
	ptsdTrtmentPlot->legend->setFillOrder(QCPLegend::foRowsFirst);

	return ptsdTrtmentPlot;
}

QCustomPlot *Skeleton::createStatePlot(const QString titleName)
{
	QMap <int, QString> stateMap;
	stateMap.insert(EXCELLENT, "Excellent");
	stateMap.insert(VERY_GOOD, "Very Good");
	stateMap.insert(GOOD, "Good");
	stateMap.insert(AVERAGE, "Average");
	stateMap.insert(BELOW_AVERAGE, "Below Average");
	stateMap.insert(POOR, "Poor");
	stateMap.insert(VERY_POOR, "Very Poor");
	stateMap.insert(WORST, "Worst");

	QCustomPlot *statePlot = new QCustomPlot;
	statePlot->plotLayout()->insertRow(0);
	//QCPTextElement *title = new QCPTextElement(statePlot, "State transition of agents", QFont("sans", 12, QFont::Bold));
	QCPTextElement *title = new QCPTextElement(statePlot, titleName, QFont("sans", 12, QFont::Bold));
	statePlot->plotLayout()->addElement(0, 0, title);

	int numBoro = numBoroInput->text().toInt();

	for(unsigned int i = 0; i < NUM_STATE_SUB_PLOTS; i++)//numBoro; i++)
	{
		QPen solidPen;
		solidPen.setColor(QColor(red[i], blue[i], green[i]));
		solidPen.setWidth(2);

		statePlot->addGraph();
		statePlot->graph(i)->setPen(solidPen);
		statePlot->graph(i)->setBrush(QColor(255, 255, 255, 0));
		//statePlot->graph(i)->setLineStyle(QCPGraph::lsLine);
		statePlot->graph(i)->setName(stateMap[i]);
		//statePlot->graph(i)->setChannelFillGraph(0);
	}

	
	statePlot->xAxis->setRange(0, numStepsInput->text().toInt());
	statePlot->xAxis->grid()->setSubGridVisible(true);
	statePlot->xAxis->setLabel("Simulation Time(Weeks)");
	statePlot->xAxis->setLabelFont(QFont("sans", 10, QFont::Bold));

	statePlot->yAxis->setRange(0, 15);//3);
	statePlot->yAxis->grid()->setSubGridVisible(true);
	statePlot->yAxis->setLabel("Percentage(%)");
	statePlot->yAxis->setLabelFont(QFont("sans", 10, QFont::Bold));

	statePlot->legend->setVisible(true);
	statePlot->legend->setBorderPen(QPen(QColor(255, 255, 255, 0.4)));
	statePlot->legend->setBrush(QColor(255, 255, 255, 0.4));
	statePlot->legend->setFillOrder(QCPLegend::foRowsFirst);

	return statePlot;
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

		agentListFinal.clear();

		setInputParameters();
		setTreatmentParameters();
		setRelapseParameters();

		file = new FileHandling;
		file->readInputFile();

		createAgents();
		createHousehold();
		//createResources();

		t = clock() - t;
		std::cout << std::endl;
		std::cout << "Calculation Time:" << ((float)t / CLOCKS_PER_SEC) << "seconds" << std::endl;
		std::cout << std::endl;

		QMessageBox::information(this, tr("Success!"), tr("Agent's variables successfully initalized!"));
		
		showStatsPb->setDisabled(false);
		runPb->setDisabled(false);
	}

}

void Skeleton::run()
{
	timeCounter = 0;
	//int count = 0;

	//for(int i = 0; i < _numSteps; i++)
	//{
	//	int count = 0; int count2 = 0;
	//	//boost::range::random_shuffle(_agentListFinal);
	//	for(auto agent = _agentListFinal.begin(); agent != _agentListFinal.end(); ++agent)
	//	{
	//		agent->executeAgentRules(this, i);
	//		if(agent->getIncomeDecline() == INCOME_LOSS_YES)
	//		{
	//			if(agent->getFinancialHelpType() == EXPEDITE) {count++;}
	//			else if(agent->getFinancialHelpType() == NORMAL) {count2++;}
	//		}

	//		if(agent->getBorough() == BRONX && agent->getAvgHHIncome() < 80000 && i == _numSteps - 1 && agent->getLeftHome() == LEFT_HOME_YES && (agent->getIncomeDecline() == INCOME_LOSS_YES || agent->getIncomeDecline() == INCOME_LOSS_NO))
	//		{
	//				_testAgent.push_back(*agent);
	//		}
	//	}

	//	std::cout << i << "\t" << count << "\t" << count2 << std::endl;
	//}

	connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
	dataTimer.start(0);

	runPb->setDisabled(true);
}

void Skeleton::reset()
{
	numAgentsInput->setText("1500000");
	numStepsInput->setText("104");
	
	//numTrialsInput->setText("50");

	sensitivityInput->setText("0.8");
	specificityInput->setText("0.8");
	screenStartTimeInput->setText("14");
	ptsdCutOffInput->setText("6.0");

	trtmentTimeInput->setText("104");
	trtmentWaitTimeInput->setText("1");
	socialWorkerTimeInput->setText("4");

	cbtResolutionCoffInput->setText("0.36");
	sprResolutionCoffInput->setText("0.2");
	naturalDecayCoffInput->setText("0.14");

	relapsePTSDxInput->setText("12.0");
	relapseTimeInput->setText("13");
	numRelapseInput->setText("1");


	for(unsigned int i = 0; i < NUM_PTSD_PLOTS; i++)
	{
		for(unsigned int j = 0; j < NUM_PTSD_SUB_PLOTS; j++)
		{

			ptsdPlot[i]->graph(j)->data()->clear();
		}
	
		ptsdPlot[i]->replot();
	}

	for(unsigned int i = 0; i < NUM_STATE_SUB_PLOTS; i++)
	{
		statePlot1->graph(i)->data()->clear();
		statePlot2->graph(i)->data()->clear();
	}

	//statePlot->replot();
	statePlot1->replot();
	statePlot2->replot();

	showStatsPb->setDisabled(true);
	runPb->setDisabled(true);
}

void Skeleton::stats()
{
	QWidget *demoWidget = new QWidget(this);
	demoWidget->setWindowFlags(Qt::Window); //creates another widget in separate window http://doc.qt.io/qt-5/qtwidgets-widgets-windowflags-example.html

	QVBoxLayout *vbox = new QVBoxLayout(demoWidget);
	QTextBrowser *statsViewer = new QTextBrowser(demoWidget);

	Statistics *stats = new Statistics(this, agentListFinal, numBoro, ageCat, raceCat, incCat, numAgents);
	stats->demographicStats();
	QString outString = stats->outString;
	statsViewer->setPlainText(outString);

	vbox->addWidget(statsViewer);
	demoWidget->setWindowTitle(tr("Demographic Stats NYC"));
	demoWidget->resize(680,300);
	demoWidget->show();

	showStatsPb->setDisabled(true);


}


void Skeleton::enableSubGroupBoxTrtment()
{
	if(steppedCareCB->isChecked() && usualCareCB->isChecked() ||  steppedCareCB->isChecked() && !usualCareCB->isChecked())
	{
		sensitivityInput->setDisabled(false);
		specificityInput->setDisabled(false);
		screenStartTimeInput->setDisabled(false);
		ptsdCutOffInput->setDisabled(false);

		trtmentTimeInput->setDisabled(false);
		trtmentWaitTimeInput->setDisabled(false);
		socialWorkerTimeInput->setDisabled(false);

		cbtResolutionCoffInput->setDisabled(false);
		sprResolutionCoffInput->setDisabled(false);
		naturalDecayCoffInput->setDisabled(false);

		costComboCBT->setDisabled(false);
		costComboSPR->setDisabled(false);

		sessionComboCBT->setDisabled(false);
		sessionComboSPR->setDisabled(false);
	}
	else if(!steppedCareCB->isChecked() && usualCareCB->isChecked())
	{
		sensitivityInput->setDisabled(true);
		specificityInput->setDisabled(true);
		screenStartTimeInput->setDisabled(false);
		ptsdCutOffInput->setDisabled(false);

		trtmentTimeInput->setDisabled(false);
		trtmentWaitTimeInput->setDisabled(false);
		socialWorkerTimeInput->setDisabled(true);

		cbtResolutionCoffInput->setDisabled(true);
		sprResolutionCoffInput->setDisabled(false);
		naturalDecayCoffInput->setDisabled(false);

		costComboCBT->setDisabled(false);
		costComboSPR->setDisabled(false);

		costComboCBT->setDisabled(true);
		costComboSPR->setDisabled(false);

		sessionComboCBT->setDisabled(true);
		sessionComboSPR->setDisabled(false);
	}
	else
	{
		sensitivityInput->setDisabled(true);
		specificityInput->setDisabled(true);
		screenStartTimeInput->setDisabled(true);
		ptsdCutOffInput->setDisabled(false);

		trtmentTimeInput->setDisabled(true);
		trtmentWaitTimeInput->setDisabled(true);
		socialWorkerTimeInput->setDisabled(true);

		cbtResolutionCoffInput->setDisabled(true);
		sprResolutionCoffInput->setDisabled(true);
		naturalDecayCoffInput->setDisabled(false);

		costComboCBT->setDisabled(true);
		costComboSPR->setDisabled(true);

		sessionComboCBT->setDisabled(true);
		sessionComboSPR->setDisabled(true);
	}
}


void Skeleton::realtimeDataSlot()
{
	countCBT = boost::assign::list_of(0)(0);
	countSPR = boost::assign::list_of(0)(0);
	countND = boost::assign::list_of(0)(0);

	countExpedite = 0;
	countNormal = 0;
	countNormalNoSW = 0;
	countNormalUC = 0;

	if(timeCounter < numSteps)
	{
		//boost::range::random_shuffle(agentListFinal);
		for(auto agent = agentListFinal.begin(); agent != agentListFinal.end(); ++agent)
		{
			agent->executeAgentRules(timeCounter);

		}

		updatePlots();
	}
	else
	{
		dataTimer.stop();
	}

	for(unsigned int i = 0; i < NUM_PTSD_PLOTS; i++)
	{
		ptsdPlot[i]->replot();
	}

	statePlot1->replot();
	statePlot2->replot();
	timeCounter++;
}

void Skeleton::setInputParameters()
{
	numAgents = numAgentsInput->text().toInt();
	numSteps = numStepsInput->text().toInt();
	//_numTrials = inNumTrials;

	numBoro = numBoroInput->text().toInt();
	ageCat = numAgeCatInput->text().toInt();
	raceCat = numRaceCatInput->text().toInt();
	incCat = numIncCatInput->text().toInt();
	subIncCat = numSubIncCatInput->text().toInt();

	//potency = boost::assign::list_of(1.0)(0.92)(0.84)(0.76);
	potency = boost::assign::list_of(1.0)(0.90)(0.80)(0.70);
	//potency = boost::assign::list_of(1.0)(0.8)(0.6)(0.4);

	for(unsigned int i = 0; i < NUM_PTSD_PLOTS; i++)
	{
		ptsdPlot[i]->xAxis->setRange(0, numSteps+10);
		ptsdPlot[i]->replot();
	}

	statePlot1->xAxis->setRange(0, numSteps+10);
	statePlot1->replot();

	statePlot2->xAxis->setRange(0, numSteps+10);
	statePlot2->replot();

	/*for(unsigned int i = 0; i < NUM_PLOTS_PTSD_TRTMENT; i++)
	{
		if(ptsdTrtmentPlot->graph(i)->data()->isEmpty() == false)
		{
			ptsdTrtmentPlot->graph(i)->data()->clear();
		}
	}*/

	//ptsdTrtmentPlot->xAxis->setRange(0, numSteps+10);
	//ptsdTrtmentPlot->replot();

	/*for(unsigned int i = 0; i < NUM_PLOTS_STATE; i++)
	{
		if(statePlot->graph(i)->data()->isEmpty() == false)
		{
			statePlot->graph(i)->data()->clear();
		}
	}*/

	//statePlot->xAxis->setRange(0, numSteps+10);
	//statePlot->replot();
}


void Skeleton::setTreatmentParameters()
{
	if(steppedCareCB->isChecked() && usualCareCB->isChecked() || steppedCareCB->isChecked() && !usualCareCB->isChecked())
	{
		steppedCare = true;
		usualCare = (usualCareCB->isChecked()) ? true : false;

		sensitivity = sensitivityInput->text().toFloat();
		specificity = specificityInput->text().toFloat();
		ptsdScreeningTime = screenStartTimeInput->text().toInt();
		ptsdxCutoff = ptsdCutOffInput->text().toFloat();

		trtmentTime = trtmentTimeInput->text().toInt();
		trtmentWaitTime = trtmentWaitTimeInput->text().toInt();
		socialWorkerTime = socialWorkerTimeInput->text().toInt();

		cbtCoeff = cbtResolutionCoffInput->text().toFloat();
		sprCoeff = sprResolutionCoffInput->text().toFloat();
		natDecayCoeff = naturalDecayCoffInput->text().toFloat();

		costCBTtrtment = costComboCBT->currentText().toInt();
		maxSessionsCBT = sessionComboCBT->currentText().toInt();

		costSPRtrtment = costComboSPR->currentText().toInt();
		maxSessionsSPR = sessionComboSPR->currentText().toInt();
	}

	else if(!steppedCareCB->isChecked() && usualCareCB->isChecked())
	{
		steppedCare = false;
		usualCare = true;

		sensitivity = 0.f;
		specificity = 0.f;
		ptsdScreeningTime = screenStartTimeInput->text().toInt();
		ptsdxCutoff = ptsdCutOffInput->text().toFloat();

		trtmentTime = trtmentTimeInput->text().toInt();
		trtmentWaitTime = trtmentWaitTimeInput->text().toInt();
		socialWorkerTime = 0;

		cbtCoeff = 0;
		sprCoeff = sprResolutionCoffInput->text().toFloat();
		natDecayCoeff = naturalDecayCoffInput->text().toFloat();

		costCBTtrtment = 0;
		maxSessionsCBT = 0;

		costSPRtrtment = costComboSPR->currentText().toInt();
		maxSessionsSPR = sessionComboSPR->currentText().toInt();
	}
	else
	{
		steppedCare = false;
		usualCare = false;

		sensitivity = 0.f;
		specificity = 0.f;
		ptsdScreeningTime = 0;
		ptsdxCutoff = ptsdCutOffInput->text().toFloat();

		trtmentTime = 0;
		trtmentWaitTime = 0;
		socialWorkerTime = 0;

		cbtCoeff = 0;
		sprCoeff = 0;
		natDecayCoeff = naturalDecayCoffInput->text().toFloat();

		costCBTtrtment = 0;
		maxSessionsCBT = 0;

		costSPRtrtment = 0;
		maxSessionsSPR = 0;
	}
}

void Skeleton::setRelapseParameters()
{
	relapsePTSDx = relapsePTSDxInput->text().toFloat();
	relapseTime = relapseTimeInput->text().toInt();
	numRelapse = numRelapseInput->text().toInt();
}

void Skeleton::createAgents()
{
	unsigned int famIdx = 0;
	Agent *myAgent = new Agent(this, file);
	for(unsigned int i = 0; i < numAgents; i++)
	{
		myAgent->setAgentAttributes(i);

		if(myAgent->getHHsize() == HHsize1){createHHSize1(myAgent, famIdx);}  //add agents with HHsize 1 to the familyList}
		else{tempAgentsMap.insert(std::pair<int, Agent> (myAgent->getKey(), *myAgent));} //creates hashtable for HHsize 2 or more 
	}

}

void Skeleton::createHousehold()
{
	//famListHHsize2.clear();

	unsigned int famIdx = famList.size()+1;

	createHHSize234Step1(famIdx); 
	createHHSize234Step2(famIdx); 
	mergeHouseholds();

	agentListFinal.reserve(numAgents);

	for(auto f = famList.begin(); f != famList.end(); ++f)
	{
		f->calcAverageFamilyIncome();
		f->calcAverageFamilySaving(file);
		f->predictSandyDamage(file);

		for(auto agent = f->agentFamList.begin(); agent != f->agentFamList.end(); ++agent)
		{
			agentListFinal.push_back(*agent);
		}

	}

	/*for(unsigned int i = 0; i < numBoro; i++)
	{
		int countIncomeLoss = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_IncomeLoss(i+1, INCOME_LOSS_YES));
		int boroPop = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Borough(i+1));
		std::cout << "% of Income Loss in population in Borough " << i+1 << "\t"  << (float)countIncomeLoss/boroPop << std::endl;
	}

	std::cout << std::endl;*/

	std::ofstream myfile;
	myfile.open("validation_proportions.txt", std::ios::app);

	for(int i = INCOME_LOSS_NO; i < INCOME_LOSS_YES+1; i++)
	{
		for(int j = MALE; j < FEMALE+1; j++)
		{
			for(int k = AGE1; k < AGE3+1; k++)
			{
				int countSP1 = boost::count_if(agentListFinal, Check_IncDecline_Gender_Age(i, j, k));
				int countSP11 = boost::count_if(agentListFinal, Check_IncDecline_Gender_Age_PTSD(i, j, k, POSITIVE));

				std::cout << i << "\t" << j << "\t" << k << "\t" << (float)countSP11/countSP1 << std::endl;
				myfile << i << "\t" << j << "\t" << k << "\t" << (float)countSP11/countSP1 << std::endl;
			}
		}
	}

	std::cout << std::endl;
	myfile << std::endl;

	for(int i = LEFT_HOME_NO; i < LEFT_HOME_YES+1; i++)
	{
		for(int j = MALE; j < FEMALE+1; j++)
		{
			for(int k = AGE1; k < AGE3+1; k++)
			{
				int countSP1 = boost::count_if(agentListFinal, Check_LeftHome_Gender_Age(i, j, k));
				int countSP11 = boost::count_if(agentListFinal, Check_LeftHome_Gender_Age_PTSD(i, j, k, POSITIVE));

				myfile << i << "\t" << j << "\t" << k << "\t" << (float)countSP11/countSP1 << std::endl;
				std::cout << i << "\t" << j << "\t" << k << "\t" << (float)countSP11/countSP1 << std::endl;
			}
		}
	}

	myfile.close();

	famListHHsize2.clear();
	famList.clear();
	
}


void Skeleton::mergeHouseholds()
{
	for(auto f1 = famListHHsize2.begin(); f1 != famListHHsize2.end(); ++f1)
	{
		famList.push_back(*f1);
	}

	//famListHHsize2.clear();
}


void Skeleton::createHHSize1(Agent *agent, unsigned int &index)
{
	index++;
	Family hhSize1(index);
	hhSize1.setFamilyAttributes(*agent);
	hhSize1.addAgent(*agent);
	famList.push_back(hhSize1);

}


void Skeleton::createHHSize234Step1(unsigned int &index)
{
	for(auto agent1 = tempAgentsMap.begin(); agent1 != tempAgentsMap.end();)
	{
		Family hhSize234(index++);
		hhSize234.setFamilyAttributes(agent1->second);
		hhSize234.addAgent(agent1->second);

		auto its = tempAgentsMap.equal_range(agent1->first);
		for(auto agent2 = its.first; agent2 != its.second;)
		{
			if(agent1->second.getID() != agent2->second.getID())
			{
				if(agent1->second.getRace() == agent2->second.getRace() && hhSize234.agentFamList.size() < agent1->second.getHHsize())
				{
					hhSize234.addAgent(agent2->second);
					agent2 = tempAgentsMap.erase(agent2);
					if(hhSize234.agentFamList.size() == agent1->second.getHHsize()){break;}
				}
				else{++agent2;}
			}
			else{++agent2;}
		}

		if(hhSize234.agentFamList.size() > HHsize1 && hhSize234.agentFamList.size() <= agent1->second.getHHsize())
		{
			famListHHsize2.push_back(hhSize234);
			agent1 = tempAgentsMap.erase(agent1);
		}
		else{++agent1;}
	}

	fillIncompleteHouseholds(); //fill the households with remaining unmatched agents if size of family list is less than household size
}


void Skeleton::createHHSize234Step2(unsigned int &index)
{
	for(auto agent1 = tempAgentsMap.begin(); agent1 != tempAgentsMap.end();)
	{
		Family hhSize234(index++);
		hhSize234.setFamilyAttributes(agent1->second);
		hhSize234.addAgent(agent1->second);

		auto its = tempAgentsMap.equal_range(agent1->first);
		for(auto agent2 = its.first; agent2 != its.second;)
		{
			if(agent1->second.getID() != agent2->second.getID())
			{
				if(hhSize234.agentFamList.size() < agent1->second.getHHsize())
				{
					hhSize234.addAgent(agent2->second);
					agent2 = tempAgentsMap.erase(agent2);
					if(hhSize234.agentFamList.size() == agent1->second.getHHsize()){break;}
				}
				else{++agent2;}
			}
			else{++agent2;}
		}

		if(hhSize234.agentFamList.size() > HHsize1 && hhSize234.agentFamList.size() <= agent1->second.getHHsize())
		{
			famListHHsize2.push_back(hhSize234);
			agent1 = tempAgentsMap.erase(agent1);
		}
		else{++agent1;}
	}

	addUnmatchedAgents(index); //Add unmatched agents to the family list
}


void Skeleton::fillIncompleteHouseholds()
{
	for(auto f1 = famListHHsize2.begin(); f1 != famListHHsize2.end();++f1)
	{
		if((f1->getFamilySize() != f1->agentFamList.size() || f1->getFamilySize() == HHsize4) && f1->getFamilySize() > HHsize2)
		{
			auto its = tempAgentsMap.equal_range(f1->getFamilyKey());
			for(auto agent = its.first; agent != its.second;)
			{
				if(f1->getFamilySize() == HHsize3 && f1->agentFamList.size() < HHsize3)
				{
					f1->addAgent(agent->second);
					agent = tempAgentsMap.erase(agent);
				}
				else if(f1->getFamilySize() == HHsize4 && f1->agentFamList.size() < MAX_PEOPLE_HHSIZE4)
				{
					f1->addAgent(agent->second);
					agent = tempAgentsMap.erase(agent);
				}
				else{++agent;}
			}
		}
	}
}

void Skeleton::addUnmatchedAgents(unsigned int &index)
{
	for(auto agent = tempAgentsMap.begin(); agent !=tempAgentsMap.end(); ++agent)
	{
		index++;

		Family remFamily(index);
		remFamily.setFamilyAttributes(agent->second);
		remFamily.addAgent(agent->second);
		famListHHsize2.push_back(remFamily);
	}

	tempAgentsMap.clear();
}

void Skeleton::updatePlots()
{
	for(unsigned int incLoss = INCOME_LOSS_NO; incLoss < INCOME_LOSS_YES+1; incLoss++)
	{
		int ptsdCountSC = boost::range::count_if(agentListFinal, Check_PTSDx_SC(incLoss));
		int ptsdResolvedCountSC = boost::range::count_if(agentListFinal, Check_Resolved_PTSD_SC(incLoss));

		int ptsdCountSCNoSW = boost::range::count_if(agentListFinal, Check_PTSDx_SC_NO_SW(incLoss));
		int ptsdResolvedCountSCNoSW = boost::range::count_if(agentListFinal, Check_Resolved_PTSD_SC_NO_SW(incLoss));

		int ptsdCountUC = boost::range::count_if(agentListFinal, Check_PTSDx_UC(incLoss));
		int ptsdResolvedCountUC = boost::range::count_if(agentListFinal, Check_Resolved_PTSD_UC(incLoss));

		ptsdPlot[incLoss-1]->graph(0)->addData(timeCounter, 100*((float)ptsdCountSC/(ptsdCountSC + ptsdResolvedCountSC)));
		ptsdPlot[incLoss-1]->graph(1)->addData(timeCounter, 100*((float)ptsdCountSCNoSW/(ptsdCountSCNoSW + ptsdResolvedCountSCNoSW)));
		ptsdPlot[incLoss-1]->graph(2)->addData(timeCounter, 100*((float)ptsdCountUC/(ptsdCountUC + ptsdResolvedCountUC)));
		ptsdPlot[incLoss-1]->graph(3)->addData(timeCounter, 100*((float)countCBT[incLoss-1]/ptsdCountSC));
		ptsdPlot[incLoss-1]->graph(4)->addData(timeCounter, 100*((float)countSPR[incLoss-1]/ptsdCountSC));
		ptsdPlot[incLoss-1]->graph(5)->addData(timeCounter, 100*((float)countND[incLoss-1]/ptsdCountSC));
	}

		/*int ptsdCountSC = 0;
		int ptsdResolvedCountSC = 0;

		int ptsdCountSCNoSW = 0;
		int ptsdResolvedCountSCNoSW = 0;

		int ptsdCountUC = 0;
		int ptsdResolvedCountUC = 0;

		int totCBT = 0, totSPR = 0, totND = 0;

	for(unsigned int incLoss = INCOME_LOSS_NO; incLoss < INCOME_LOSS_YES+1; incLoss++)
	{
		ptsdCountSC += boost::range::count_if(agentListFinal, Check_PTSDx_SC(incLoss));
		ptsdResolvedCountSC += boost::range::count_if(agentListFinal, Check_Resolved_PTSD_SC(incLoss));

		ptsdCountSCNoSW += boost::range::count_if(agentListFinal, Check_PTSDx_SC_NO_SW(incLoss));
		ptsdResolvedCountSCNoSW += boost::range::count_if(agentListFinal, Check_Resolved_PTSD_SC_NO_SW(incLoss));

		ptsdCountUC += boost::range::count_if(agentListFinal, Check_PTSDx_UC(incLoss));
		ptsdResolvedCountUC += boost::range::count_if(agentListFinal, Check_Resolved_PTSD_UC(incLoss));

	}

	ptsdPlot[0]->graph(0)->addData(timeCounter, 100*((float)ptsdCountSC/(ptsdCountSC + ptsdResolvedCountSC)));
	ptsdPlot[0]->graph(1)->addData(timeCounter, 100*((float)ptsdCountSCNoSW/(ptsdCountSCNoSW + ptsdResolvedCountSCNoSW)));
	ptsdPlot[0]->graph(2)->addData(timeCounter, 100*((float)ptsdCountUC/(ptsdCountUC + ptsdResolvedCountUC)));
	ptsdPlot[0]->graph(3)->addData(timeCounter, 100*((float)totCBT/(ptsdCountSC+ptsdCountSCNoSW)));
	ptsdPlot[0]->graph(4)->addData(timeCounter, 100*((float)totSPR/(ptsdCountSC+ptsdCountSCNoSW)));
	ptsdPlot[0]->graph(5)->addData(timeCounter, 100*((float)totND/(ptsdCountSC+ptsdCountSCNoSW)));*/

	std::cout << timeCounter << "\t" << countExpedite << "\t" << countNormal << "\t" << countNormalNoSW << "\t" << countNormalUC << std::endl;

	//boost::container::vector<int> stateCountSC(NUM_STATE_SUB_PLOTS, 0);
	//boost::container::vector<int> stateCountUC(NUM_STATE_SUB_PLOTS, 0);

	boost::array<unsigned int, NUM_STATE_SUB_PLOTS> stateCountSC = {0, 0, 0, 0, 0, 0, 0, 0};
	boost::array<unsigned int, NUM_STATE_SUB_PLOTS> stateCountUC = {0, 0, 0, 0, 0, 0, 0, 8};
	
	for(unsigned int i = 0; i < NUM_STATE_SUB_PLOTS; i++)
	{
		stateCountSC[i] = boost::range::count_if(agentListFinal, Check_State_SC(i));
		//stateCountSC[i] = boost::range::count_if(agentListFinal, Check_State_SC_NoSW(i));
		stateCountUC[i] = boost::range::count_if(agentListFinal, Check_State_UC(i));
		
		statePlot1->graph(i)->addData(timeCounter, 100*((float)stateCountSC[i]/numAgents));
		statePlot2->graph(i)->addData(timeCounter, 100*((float)stateCountUC[i]/numAgents));
	}
}












