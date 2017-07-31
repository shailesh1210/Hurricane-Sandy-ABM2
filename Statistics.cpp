#include "Statistics.h"
#include "skeleton.h"
#include "Family.h"
#include "Agent.h"
#include <QMap>
#include <boost/range/algorithm/count_if.hpp>



Statistics::Statistics(QWidget *parent, AgentList &_agentListFinal, int inNumBoro, int inNumAge, int inNumRace, int inNumIncome, int inNumAgents) :
	agentListFinal(_agentListFinal), numBoro(inNumBoro), numAge(inNumAge), numRace(inNumRace), numIncome(inNumIncome), numAgents(inNumAgents), numDamageType(3), 
	numLeftHome(2), numPTSDstatus(2), numFinLossType(4), populationCount(inNumBoro, 0), ageCount(inNumBoro*inNumAge, 0), raceCount(inNumBoro*inNumRace, 0), 
	incCount(inNumBoro*inNumIncome, 0), elecLossCount(inNumBoro, 0), waterLossCount(inNumBoro, 0), heatLossCount(inNumBoro, 0), damageTypeCount(3*inNumBoro, 0), 
	leftHomeCount(2*inNumBoro, 0), ptsdStatusCount(2*inNumBoro, 0), finLossTypeCount(4*inNumBoro, 0), textTicker(new QCPAxisTickerText)
{
	QDesktopWidget *m = QApplication::desktop();
	QRect desk_rect = m->screenGeometry(m->screenNumber(QCursor::pos()));
	int desk_x = desk_rect.width();
	int desk_y = desk_rect.height();
	
	plot1 = new QWidget(parent);
	plot1->setWindowFlags(Qt::Window);
	plot1->resize(1024, 800);
	plot1->setWindowTitle("Demograhic Charts NYC");

	int x1 = plot1->width();
	int y1 = plot1->height();

	plot1->move(desk_x/2 - x1/2 + desk_rect.left(), desk_y/2 - y1/2 + desk_rect.top());

	plot2 = new QWidget(parent);
	plot2->setWindowFlags(Qt::Window);
	plot2->resize(1024, 800);
	plot2->setWindowTitle("Sandy Damage Charts NYC");

	//initalizes and sets title to population bar graph
	popChart = new QCustomPlot;
	popChart->plotLayout()->insertRow(0);
	QCPTextElement *title = new QCPTextElement(popChart, "Population of NYC by Borough", QFont("sans", 14, QFont::Bold));
	popChart->plotLayout()->addElement(0, 0, title);

	//initalizes and sets title to age bar graph
	ageChart = new QCustomPlot;
	ageChart->plotLayout()->insertRow(0);
	QCPTextElement *title1 = new QCPTextElement(ageChart, "Age Categories by Borough", QFont("sans", 14, QFont::Bold));
	ageChart->plotLayout()->addElement(0, 0, title1);

	//initalizes and sets title to race bar graph
	raceChart = new QCustomPlot;
	raceChart->plotLayout()->insertRow(0);
	QCPTextElement *title2 = new QCPTextElement(raceChart, "Race Categories by Borough", QFont("sans", 14, QFont::Bold));
	raceChart->plotLayout()->addElement(0, 0, title2);

	//initalizes and sets title to income bar graph
	incomeChart = new QCustomPlot;
	incomeChart->plotLayout()->insertRow(0);
	QCPTextElement *title3 = new QCPTextElement(incomeChart, "Income Categories by Borough", QFont("sans", 14, QFont::Bold));
	incomeChart->plotLayout()->addElement(0, 0, title3);

	//intializes and set title to utility loss bar graph
	utilityLossChart = new QCustomPlot;
	utilityLossChart->plotLayout()->insertRow(0);
	QCPTextElement *title4 = new QCPTextElement(utilityLossChart, "Utility loss by Borough", QFont("sans", 14, QFont::Bold));
	utilityLossChart->plotLayout()->addElement(0, 0, title4);

	displacementChart = new QCustomPlot;
	displacementChart->plotLayout()->insertRow(0);
	QCPTextElement *title5 = new QCPTextElement(displacementChart, "Temporary displacement by borough", QFont("sans", 14, QFont::Bold));
	displacementChart->plotLayout()->addElement(0, 0, title5);

	damageChart = new QCustomPlot;
	damageChart->plotLayout()->insertRow(0);
	QCPTextElement *title6 = new QCPTextElement(damageChart, "Damage type by Borough", QFont("sans", 14, QFont::Bold));
	damageChart->plotLayout()->addElement(0, 0, title6);

	financialLossChart = new QCustomPlot;
	financialLossChart->plotLayout()->insertRow(0);
	QCPTextElement *title7 = new QCPTextElement(financialLossChart, "Financial loss by Borough", QFont("sans", 14, QFont::Bold));
	financialLossChart->plotLayout()->addElement(0, 0, title7);


	//color codes for bar chart
	red = QVector<int>() << 204 << 102 << 255 << 0;
	green = QVector<int>() << 0 << 178  << 128 << 153;
	blue = QVector<int>() << 0 << 255 << 0 << 76;

	//customized ticks for x-axis for bar graph
	boroLabels << "Bronx" << "Brooklyn" << "Manhattan" << "Queens" << "Staten";
	ticks << 1 << 2 << 3 << 4 << 5;
	textTicker->addTicks(ticks, boroLabels);


	/*for(auto fam = familyList.begin(); fam != familyList.end(); ++fam)
	{
		for(auto agent = fam->_agentFamList.begin(); agent != fam->_agentFamList.end(); ++agent)
		{
			agentListFinal.push_back(*agent);
		}
	}*/
}

Statistics::~Statistics()
{

}

void Statistics::demographicStats()
{
	QMap <int, QString> boroNames;
	boroNames.insert(0, "Bronx");  
	boroNames.insert(1, "Brooklyn");
	boroNames.insert(2, "Manhattan");
	boroNames.insert(3, "Queens");
	boroNames.insert(4, "Staten");

	QCPBars *popBar = new QCPBars(popChart->xAxis, popChart->yAxis);
	setBarGraphProperties(popBar, 0.75, 0);

	outString += "Borough \tPopulation \tCategory \tCount \tProportion \n\n";
	for(unsigned int i = 0; i < numBoro; i++)
	{
		populationCount[i] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Borough(i+1));
		outString += boroNames[i];

		popBar->addData(i+1, 100*populationCount[i]/numAgents);

		counterAgeByBorough(i);
		counterRaceByBorough(i);
		counterIncomeByBorough(i);

		counterUtilityLossByBorough(i);
		counterDamageTypeByBorough(i);
		counterDisplacementByBorough(i);
		counterFinancialLossByBorough(i);
		counterPTSDstatusByBorough(i);

		outString += "\n";
	}

	setAxisDemoBarGraph();
	setAxisSandyDamageBarGraph();

	displayBarGraph(plot1, popChart, ageChart, raceChart, incomeChart); //displays bar graphs for plot1 (demograhic stats)
	displayBarGraph(plot2, damageChart, utilityLossChart, displacementChart, financialLossChart);
}

void Statistics::counterAgeByBorough(unsigned int i)
{
	QMap<int, QString>ageCatString;
	ageCatString.insert(0, "18 - 34");
	ageCatString.insert(1, "34 - 64");
	ageCatString.insert(2, "65+");

	QCPBarsGroup *group = new QCPBarsGroup(ageChart);

	QString strng1 = QString::number(populationCount[i]);
	QString strng2 = "";

	for(unsigned int j = 0; j < numAge; j++)
	{
		QCPBars *ageBar = new QCPBars(ageChart->xAxis, ageChart->yAxis);
		ageBar->setName("Age " + ageCatString[j]);

		//ageCount[numAge*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Age(i+1, j+1));
		ageCount[numAge*i+j] = boost::range::count_if(agentListFinal, Check_Age(i+1, j+1));
		
		float perAgeBoro = (float)ageCount[numAge*i+j] / populationCount[i];

		QString popCount = (j != 0) ? strng2 : strng1;
		outString += "\t" + popCount + "\t" + ageCatString[j] + "\t" + QString::number(ageCount[numAge*i+j]) + "\t" + QString::number(perAgeBoro) + "\n";

		ageBar->addData(i+1, 100*perAgeBoro);
		ageBar->setBarsGroup(group);

		setBarGraphProperties(ageBar, 0.2, j);
	}
}

void Statistics::counterRaceByBorough(unsigned int i)
{
	QMap<int, QString>raceCatString;
	raceCatString.insert(0, "White NH");
	raceCatString.insert(1, "Black NH");
	raceCatString.insert(2, "Other NH");
	raceCatString.insert(3, "Hispanic");

	QCPBarsGroup *group = new QCPBarsGroup(raceChart);
	
	for(unsigned int j = 0; j < numRace; j++)
	{
		QCPBars *raceBars = new QCPBars(raceChart->xAxis, raceChart->yAxis);
		raceBars->setName(raceCatString[j]);

		//raceCount[numRace*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Race(i+1, j+1));
		raceCount[numRace*i+j] = boost::range::count_if(agentListFinal, Check_Race(i+1, j+1));
		float perRaceBoro = (float)raceCount[numRace*i+j] / populationCount[i];
		outString +=   "\t\t" + raceCatString[j] + "\t" + QString::number(raceCount[numRace*i+j]) + "\t" +  QString::number(perRaceBoro) + "\n";

		raceBars->addData(i+1, 100*perRaceBoro);
		raceBars->setBarsGroup(group);

		setBarGraphProperties(raceBars, 0.15, j);
	}
}

void Statistics::counterIncomeByBorough(unsigned int i)
{
	QMap<int, QString> incomeCatString;
	incomeCatString.insert(0, "Inc 0-40K");
	incomeCatString.insert(1, "Inc 40K-80K");
	incomeCatString.insert(2, "Inc 80K+");

	QCPBarsGroup *group = new QCPBarsGroup(incomeChart);

	for(unsigned int j = 0; j < numIncome; j++)
	{
		QCPBars *incomeBar = new QCPBars(incomeChart->xAxis, incomeChart->yAxis);
		incomeBar->setName(incomeCatString[j]);

		//incCount[numIncome*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Income(i+1, j+1));
		incCount[numIncome*i+j] = boost::range::count_if(agentListFinal, Check_Income(i+1, j+1));
		float perIncomeBoro = (float)incCount[numIncome*i+j] / populationCount[i];
		outString +=   "\t\t" + incomeCatString[j] + "\t" + QString::number(incCount[numIncome*i+j]) + "\t" +  QString::number(perIncomeBoro) + "\n";

		incomeBar->addData(i+1, 100*perIncomeBoro);
		incomeBar->setBarsGroup(group);

		setBarGraphProperties(incomeBar, 0.2, j);
	}
}

void Statistics::counterUtilityLossByBorough(unsigned int i)
{

	/*elecLossCount[i] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Electric_Loss(i+1, ELECTRIC_LOSS));
	heatLossCount[i] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Heat_Loss(i+1, HEAT_LOSS));
	waterLossCount[i] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Water_Loss(i+1, WATER_LOSS));*/

	elecLossCount[i] = boost::range::count_if(agentListFinal, Check_Electric_Loss(i+1, ELECTRIC_LOSS));
	heatLossCount[i] = boost::range::count_if(agentListFinal, Check_Heat_Loss(i+1, HEAT_LOSS));
	waterLossCount[i] = boost::range::count_if(agentListFinal, Check_Water_Loss(i+1, WATER_LOSS));

	outString += "\t\tElectric Loss\t" + QString::number(elecLossCount[i]) + "\t" + QString::number((float)elecLossCount[i]/populationCount[i]) + "\n"; 
	outString += "\t\tHeat Loss\t" + QString::number(heatLossCount[i]) + "\t" + QString::number((float)heatLossCount[i]/populationCount[i]) + "\n";
	outString += "\t\tWater Loss\t" + QString::number(waterLossCount[i]) + "\t" + QString::number((float)waterLossCount[i]/populationCount[i]) + "\n";

	QCPBarsGroup *group = new QCPBarsGroup(utilityLossChart);

	QCPBars *utilityLossBar = new QCPBars(utilityLossChart->xAxis, utilityLossChart->yAxis);
	utilityLossBar->setName("Electric Loss");
	utilityLossBar->addData(i+1, 100*(float)elecLossCount[i]/populationCount[i]);
	utilityLossBar->setBarsGroup(group);
	setBarGraphProperties(utilityLossBar, 0.2, 0);

	utilityLossBar = new QCPBars(utilityLossChart->xAxis, utilityLossChart->yAxis);
	utilityLossBar->setName("Heat Loss");
	utilityLossBar->addData(i+1, 100*(float)heatLossCount[i]/populationCount[i]);
	utilityLossBar->setBarsGroup(group);
	setBarGraphProperties(utilityLossBar, 0.2, 1);

	utilityLossBar = new QCPBars(utilityLossChart->xAxis, utilityLossChart->yAxis);
	utilityLossBar->setName("Water Loss");
	utilityLossBar->addData(i+1, 100*(float)waterLossCount[i]/populationCount[i]);
	utilityLossBar->setBarsGroup(group);
	setBarGraphProperties(utilityLossBar, 0.2, 2);

}

void Statistics::counterDamageTypeByBorough(unsigned int i)
{
	QMap<int, QString> damageTypeString;
	damageTypeString.insert(0, "No damage");
	damageTypeString.insert(1, "Moderate");
	damageTypeString.insert(2, "Severe");

	QCPBarsGroup *group = new QCPBarsGroup(damageChart);

	for(unsigned int j = 0; j < numDamageType; j++)
	{
		QCPBars *damageBar = new QCPBars(damageChart->xAxis, damageChart->yAxis);
		damageBar->setName(damageTypeString[j]);

		//damageTypeCount[numDamageType*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Damage_Type(i+1, j+1));
		damageTypeCount[numDamageType*i+j] = boost::range::count_if(agentListFinal, Check_Damage_Type(i+1, j+1));
		float perDamageBoro = (float)damageTypeCount[numDamageType*i+j] / populationCount[i];
		outString +=   "\t\t" + damageTypeString[j] + "\t" + QString::number(damageTypeCount[numDamageType*i+j]) + "\t" +  QString::number(perDamageBoro) + "\n";

		damageBar->addData(i+1, 100*perDamageBoro);
		damageBar->setBarsGroup(group);

		setBarGraphProperties(damageBar, 0.2, j);
	}
}

void Statistics::counterDisplacementByBorough(unsigned int i)
{
	QMap <int, QString> leftHomeString;
	leftHomeString.insert(0, "Left Home(No)");
	leftHomeString.insert(1, "Left Home(Yes)");

	QCPBarsGroup *group = new QCPBarsGroup(displacementChart);

	for(unsigned int j = 0; j < numLeftHome; j++)
	{
		QCPBars *displacementBar = new QCPBars(displacementChart->xAxis, displacementChart->yAxis);
		displacementBar->setName(leftHomeString[j]);
		
		//leftHomeCount[numLeftHome*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Displacement(i+1, j));
		leftHomeCount[numLeftHome*i+j] = boost::range::count_if(agentListFinal, Check_Displacement(i+1, j));
		float perLeftHome = (float)leftHomeCount[numLeftHome*i+j] / populationCount[i];
		outString +=   "\t\t" + leftHomeString[j] + "\t" + QString::number(leftHomeCount[numLeftHome*i+j]) + "\t" +  QString::number(perLeftHome) + "\n";

		displacementBar->addData(i+1, 100*perLeftHome);
		displacementBar->setBarsGroup(group);

		setBarGraphProperties(displacementBar, 0.25, j);
	}
}

void Statistics::counterFinancialLossByBorough(unsigned int i)
{
	QMap<int, QString> finLossString;
	finLossString.insert(0, "No Loss");
	finLossString.insert(1, "$1-$10K");
	finLossString.insert(2, "$10K-$25K");
	finLossString.insert(3, "$25K-$40K");

	QCPBarsGroup *group = new QCPBarsGroup(financialLossChart);

	for(unsigned int j = 0; j < numFinLossType; j++)
	{
		QCPBars *finLossBars = new QCPBars(financialLossChart->xAxis, financialLossChart->yAxis);
		finLossBars->setName(finLossString[j]);

		//finLossTypeCount[numFinLossType*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_Financial_loss(i+1, j));
		finLossTypeCount[numFinLossType*i+j] = boost::range::count_if(agentListFinal, Check_Financial_loss(i+1, j));
		float perFinLoss = (float)finLossTypeCount[numFinLossType*i+j]/populationCount[i];
		outString += "\t\t" + finLossString[j] + "\t" + QString::number(finLossTypeCount[numFinLossType*i+j]) + "\t" + QString::number(perFinLoss) + "\n";

		finLossBars->addData(i+1, 100*perFinLoss);
		finLossBars->setBarsGroup(group);

		setBarGraphProperties(finLossBars, 0.15, j);
	}
}

void Statistics::counterPTSDstatusByBorough(unsigned int i)
{
	QMap <int, QString> ptsdString;
	ptsdString.insert(0, "PTSD -ve");
	ptsdString.insert(1, "PTSD +ve");

	for(unsigned int j = 0; j < numPTSDstatus; j++)
	{
		//ptsdStatusCount[numPTSDstatus*i+j] = std::count_if(agentListFinal.begin(), agentListFinal.end(), Check_PTSD_Status(i+1, j));
		ptsdStatusCount[numPTSDstatus*i+j] = boost::range::count_if(agentListFinal, Check_PTSD_Status(i+1, j));
		float perPTSD = (float)ptsdStatusCount[numPTSDstatus*i+j]/populationCount[i];
		outString += "\t\t" + ptsdString[j] + "\t" + QString::number(ptsdStatusCount[numPTSDstatus*i+j]) + "\t" + QString::number(perPTSD) + "\n";
	}
}

void Statistics::setBarGraphProperties(QCPBars *bar, double width, unsigned int j)
{
	bar->setAntialiased(false);
	bar->setPen(QColor(red[j], green[j], blue[j]));
	bar->setBrush(QColor(red[j], green[j], blue[j]).lighter(100));
	bar->setWidth(width);
}

void Statistics::setAxisDemoBarGraph()
{
	setAxisProperties(popChart, "Borough NYC", "% Population", "right", false, 0, 0); //sets axis labels and properties for population chart
	setAxisProperties(ageChart, "Borough NYC", "% Age Category", "right", true, numAge, ageCount.size()); //sets axis labels and properties for age chart
	setAxisProperties(raceChart, "Borough NYC", "% Race Category", "right", true, numRace, raceCount.size()); //sets axis labels and properties for race chart;
	setAxisProperties(incomeChart, "Borough NYC", "% Income Category", "right", true, numIncome, incCount.size()); // sets axis labels and properties for income chart
}

void Statistics::setAxisSandyDamageBarGraph()
{
	setAxisProperties(utilityLossChart, "Borough NYC", "% Utility Loss", "left", true, 3, elecLossCount.size()+heatLossCount.size()+waterLossCount.size());
	setAxisProperties(damageChart, "Borough NYC", "% Damage Type", "right", true, numDamageType, damageTypeCount.size());
	setAxisProperties(displacementChart, "Borough NYC", "% Displacement", "right", true, numLeftHome, leftHomeCount.size());
	setAxisProperties(financialLossChart, "Borough NYC", "% Financial Loss", "right", true, numFinLossType, finLossTypeCount.size());
}

void Statistics::setAxisProperties(QCustomPlot *plot, const QString xLabel, const QString yLabel, const QString legendPosition, bool legendShow, int index, int size)
{
	//Prepare x-axis with labels
	plot->xAxis->setTicker(textTicker);
	plot->xAxis->setRange(0, 6);
	plot->xAxis->setLabel(xLabel);
	plot->xAxis->setLabelFont(QFont("sans", 12, QFont::Bold));

	//Prepare y-axis
	plot->yAxis->setRange(0, 100);
	plot->yAxis->grid()->setSubGridVisible(true);
	plot->yAxis->setLabel(yLabel);
	plot->yAxis->setLabelFont(QFont("sans", 12, QFont::Bold));

	plot->legend->setVisible(legendShow);
	plot->legend->setBorderPen(QPen(QColor(255, 255, 255, 0.4)));
	plot->legend->setBrush(QColor(255, 255, 255, 0.4));
	plot->legend->setFillOrder(QCPLegend::foRowsFirst);
	
	if(legendPosition == "right") {plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);}
	else if(legendPosition == "left") {plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);}

	while(index < size && legendShow){plot->legend->removeAt(index++);}
	
}

void Statistics::displayBarGraph(QWidget *plot, QCustomPlot *bar1, QCustomPlot *bar2, QCustomPlot *bar3, QCustomPlot *bar4)
{
	QHBoxLayout *mainLayout = new QHBoxLayout(plot);
	QHBoxLayout *leftLayout = new QHBoxLayout;
	QHBoxLayout *rightLayout = new QHBoxLayout;

	QVBoxLayout *vbox1 = new QVBoxLayout;
	QVBoxLayout *vbox2 = new QVBoxLayout;

	vbox1->setSpacing(10);
	vbox1->addWidget(bar1);
	vbox1->addWidget(bar2);

	vbox2->setSpacing(10);
	vbox2->addWidget(bar3);
	vbox2->addWidget(bar4);

	leftLayout->addLayout(vbox1);
	rightLayout->addLayout(vbox2);

	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);

	plot->show();
}