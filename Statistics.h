#ifndef STATISTICS_H
#define STATISTICS_H

#include "Parameters.h"
#include "Agent.h"
#include <QString>
#include <QVector>
#include "qcustomplot.h"

class Skeleton;
class Family;
class QWidget;

class Statistics
{
public:
	Statistics(QWidget *parent, AgentList &_agentListFinal, int _numBoro, int _numAge, int _numRace, int _numIncome, int _numAgents);
	virtual ~Statistics();

	void demographicStats();
	QString outString;

protected:
	void counterAgeByBorough(unsigned int i);
	void counterRaceByBorough(unsigned int i);
	void counterIncomeByBorough(unsigned int i);
	void counterUtilityLossByBorough(unsigned int i);
	void counterDamageTypeByBorough(unsigned int i);
	void counterDisplacementByBorough(unsigned int i);
	void counterFinancialLossByBorough(unsigned int i);
	void counterPTSDstatusByBorough(unsigned int i);

	void setBarGraphProperties(QCPBars *bar, double width, unsigned int j);
	void setAxisDemoBarGraph();
	void setAxisSandyDamageBarGraph();
	void setAxisProperties(QCustomPlot *plot, const QString xLabel, const QString yLabel, const QString legendPosition, bool legendShow, int index, int size);
	void displayBarGraph(QWidget *plot, QCustomPlot *bar1, QCustomPlot *bar2, QCustomPlot *bar3, QCustomPlot *bar4);
	
protected:
	AgentList agentListFinal;
	//FamilyList familyList;

	int numBoro;
	int numAge;
	int numRace;
	int numIncome;
	int numAgents;
	int numDamageType;
	int numLeftHome;
	int numPTSDstatus;
	int numFinLossType;

	/*std::vector<int> populationCount;
	std::vector<int> ageCount;
	std::vector<int> raceCount;
	std::vector<int> incCount;
	std::vector<int> elecLossCount;
	std::vector<int> waterLossCount;
	std::vector<int> heatLossCount;
	std::vector<int> damageTypeCount;
	std::vector<int> leftHomeCount;
	std::vector<int> ptsdStatusCount;
	std::vector<int> finLossTypeCount;*/

	boost::container::vector<int> populationCount;
	boost::container::vector<int> ageCount;
	boost::container::vector<int> raceCount;
	boost::container::vector<int> incCount;
	boost::container::vector<int> elecLossCount;
	boost::container::vector<int> waterLossCount;
	boost::container::vector<int> heatLossCount;
	boost::container::vector<int> damageTypeCount;
	boost::container::vector<int> leftHomeCount;
	boost::container::vector<int> ptsdStatusCount;
	boost::container::vector<int> finLossTypeCount;
	

	QSharedPointer<QCPAxisTickerText> textTicker;
	
	//AgentList agentListFinal;

	QWidget *plot1;
	QCustomPlot *popChart;
	QCustomPlot *ageChart;
	QCustomPlot *raceChart;
	QCustomPlot *incomeChart;

	QWidget *plot2;
	QCustomPlot *utilityLossChart;
	QCustomPlot *displacementChart;
	QCustomPlot *damageChart;
	QCustomPlot *financialLossChart;
	
	QVector<int> red;
	QVector<int> green;
	QVector<int> blue;

	QVector <QString> boroLabels;
	QVector <double> ticks;
};

struct Check_Borough
{
	int borough;
	Check_Borough(int inBoro) : borough(inBoro){}

	bool operator()(Agent &a){
		return a.getBorough() == borough;
	}
};

struct Check_Race
{
	int borough;
	int race;

	Check_Race(int inBoro, int inRace) : borough(inBoro), race(inRace){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getRace() == race);
	}
};

struct Check_Age
{
	int borough;
	int ageCat;

	Check_Age(int inBoro, int inAge) : borough(inBoro), ageCat(inAge){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getAgeCat() == ageCat);
	}
};

struct Check_Income
{
	int borough;
	int income;

	Check_Income(int inBoro, int inIncome) : borough(inBoro), income(inIncome){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getIncome() == income);
	}
};

struct Check_Electric_Loss
{
	int borough;
	int elecLoss;

	Check_Electric_Loss(int inBoro, int inElecLoss) : borough(inBoro), elecLoss(inElecLoss){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getElectricLoss() == elecLoss);
	}
};

struct Check_Heat_Loss
{
	int borough;
	int heatLoss;

	Check_Heat_Loss(int inBoro, int inHeatLoss) : borough(inBoro), heatLoss(inHeatLoss){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getHeatLoss() == heatLoss);
	}
};

struct Check_Water_Loss
{
	int borough;
	int waterLoss;

	Check_Water_Loss(int inBoro, int inWaterLoss) : borough(inBoro), waterLoss(inWaterLoss){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getWaterLoss() == waterLoss);
	}
};

struct Check_Damage_Type
{
	int borough;
	int damageType;

	Check_Damage_Type(int inBoro, int inDamage) : borough(inBoro), damageType(inDamage){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getDamage() == damageType);
	}
};

struct Check_Displacement
{
	int borough;
	int leftHome;

	Check_Displacement(int inBoro, int inLeftHome) : borough(inBoro), leftHome(inLeftHome){}

	bool operator() (Agent &a){
		return(a.getBorough() == borough && a.getLeftHome() == leftHome);
	}
};

struct Check_Financial_loss
{
	int borough;
	int finLossType;

	Check_Financial_loss(int inBoro, int inFinLoss) : borough(inBoro), finLossType(inFinLoss){}

	bool operator () (Agent &a){
		return(a.getBorough() == borough && a.getFinancialLossType() == finLossType);
	}
};

struct Check_PTSD_Status
{
	int borough;
	int statusPTSD;

	Check_PTSD_Status(int inBoro, int inPTSDstatus) : borough(inBoro), statusPTSD(inPTSDstatus){}

	bool operator() (Agent &a) {
		return(a.getBorough() == borough && a.getPTSDstatus() == statusPTSD);
	}
};



#endif STATISTICS_H