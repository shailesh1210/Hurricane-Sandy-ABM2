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
class Agent;

class Statistics
{
public:
	Statistics(QWidget *parent, AgentList &_agentListFinal, int inNumBoro, int inNumAge, int inNumRace, int inNumIncome, int inNumAgents);
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

	unsigned short int numBoro;
	unsigned short int numAge;
	unsigned short int numRace;
	unsigned short int numIncome;
	int numAgents;
	unsigned short int numDamageType;
	unsigned short int numLeftHome;
	unsigned short int numPTSDstatus;
	unsigned short int numFinLossType;

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
	unsigned short int borough;
	Check_Borough(int inBoro) : borough(inBoro){}

	bool operator()(Agent &a){
		return a.getBorough() == borough;
	}
};

struct Check_Race
{
	unsigned short int borough;
	unsigned short int race;

	Check_Race(int inBoro, int inRace) : borough(inBoro), race(inRace){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getRace() == race);
	}
};

struct Check_Age
{
	unsigned short int borough;
	unsigned short int ageCat;

	Check_Age(int inBoro, int inAge) : borough(inBoro), ageCat(inAge){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getAgeCat() == ageCat);
	}
};

struct Check_Income
{
	unsigned short int borough;
	int income;

	Check_Income(unsigned short int inBoro, int inIncome) : borough(inBoro), income(inIncome){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getIncome() == income);
	}
};

struct Check_Electric_Loss
{
	unsigned short int borough;
	unsigned short int elecLoss;

	Check_Electric_Loss(unsigned short int inBoro, unsigned short int inElecLoss) : borough(inBoro), elecLoss(inElecLoss){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getElectricLoss() == elecLoss);
	}
};

struct Check_Heat_Loss
{
	unsigned short int borough;
	unsigned short int heatLoss;

	Check_Heat_Loss(unsigned short int inBoro, unsigned short int inHeatLoss) : borough(inBoro), heatLoss(inHeatLoss){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getHeatLoss() == heatLoss);
	}
};

struct Check_Water_Loss
{
	unsigned short int borough;
	unsigned short int waterLoss;

	Check_Water_Loss(unsigned short int inBoro, unsigned short int inWaterLoss) : borough(inBoro), waterLoss(inWaterLoss){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getWaterLoss() == waterLoss);
	}
};

struct Check_Damage_Type
{
	unsigned short int borough;
	unsigned short int damageType;

	Check_Damage_Type(unsigned short int inBoro, unsigned short int inDamage) : borough(inBoro), damageType(inDamage){}

	bool operator() (Agent &a){
		return (a.getBorough() == borough && a.getDamage() == damageType);
	}
};

struct Check_Displacement
{
	unsigned short int borough;
	unsigned short int leftHome;

	Check_Displacement(unsigned short int inBoro, unsigned short int inLeftHome) : borough(inBoro), leftHome(inLeftHome){}

	bool operator() (Agent &a){
		return(a.getBorough() == borough && a.getLeftHome() == leftHome);
	}
};

struct Check_Financial_loss
{
	unsigned short int borough;
	unsigned short int finLossType;

	Check_Financial_loss(unsigned short int inBoro, unsigned short int inFinLoss) : borough(inBoro), finLossType(inFinLoss){}

	bool operator () (Agent &a){
		return(a.getBorough() == borough && a.getFinancialLossType() == finLossType);
	}
};

struct Check_PTSD_Status
{
	unsigned short int borough;
	unsigned short int statusPTSD;

	Check_PTSD_Status(unsigned short int inBoro, unsigned short int inPTSDstatus) : borough(inBoro), statusPTSD(inPTSDstatus){}

	bool operator() (Agent &a) {
		return(a.getBorough() == borough && a.getPTSDstatus() == statusPTSD);
	}
};

struct Check_PTSDx_SC
{
	unsigned short int incomeLoss;

	Check_PTSDx_SC(unsigned short int inIncomeLoss) :incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getPTSDsymptom() > PTSD_CUT_OFF && a.getIncomeDeclineT0()== incomeLoss);
	}
};

struct Check_Resolved_PTSD_SC
{
	unsigned short int incomeLoss;
	Check_Resolved_PTSD_SC(unsigned short int inIncomeLoss) : incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getPTSDresolvedSC() && a.getInitialPTSDsymptom() > PTSD_CUT_OFF  && a.getIncomeDeclineT0()== incomeLoss);
	}
};

struct Check_PTSDx_SC_NO_SW
{
	unsigned short int incomeLoss;

	Check_PTSDx_SC_NO_SW(unsigned short int inIncomeLoss) : incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getPTSDsymptomNoSW() > PTSD_CUT_OFF && a.getIncomeDeclineT0()== incomeLoss);
	}
};

struct Check_Resolved_PTSD_SC_NO_SW
{
	unsigned short int incomeLoss;
	Check_Resolved_PTSD_SC_NO_SW(unsigned short int inIncomeLoss) : incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getPTSDresolvedSCNoSW() && a.getInitialPTSDsymptom() > PTSD_CUT_OFF && a.getIncomeDeclineT0()== incomeLoss);
	}
};

struct Check_PTSDx_UC
{
	unsigned short int incomeLoss;

	Check_PTSDx_UC(unsigned short int inIncomeLoss) :incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getPTSDsymptomUC() > PTSD_CUT_OFF && a.getIncomeDeclineT0()== incomeLoss);
	}
};

struct Check_Resolved_PTSD_UC
{
	unsigned short int incomeLoss;
	Check_Resolved_PTSD_UC(unsigned short int inIncomeLoss) : incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getPTSDresolvedUC() && a.getInitialPTSDsymptom() > PTSD_CUT_OFF && a.getIncomeDeclineT0()== incomeLoss);
	}
};

struct Check_IncomeLoss
{
	unsigned short int incomeLoss;
	unsigned short int borough;
	Check_IncomeLoss(unsigned short int inBoro, unsigned short int inIncomeLoss) : borough(inBoro), incomeLoss(inIncomeLoss){}

	bool operator() (Agent &a) {
		return(a.getBorough() == borough && a.getIncomeDeclineT0() == incomeLoss);
	}
};



struct Check_State_SC
{
	unsigned short int state;

	Check_State_SC(unsigned short int inState) : state(inState){}

	bool operator() (Agent &a){
		return a.getAgentState() == state;
	}
};

struct Check_State_SC_NoSW
{
	unsigned short int state;

	Check_State_SC_NoSW(unsigned short int inState) : state(inState){}

	bool operator() (Agent &a){
		return a.getAgentStateNoSW() == state;
	}
};

struct Check_State_UC
{
	unsigned short int state;

	Check_State_UC(unsigned short int inState) : state(inState){}

	bool operator() (Agent &a){
		return a.getAgentStateUC() == state;
	}
};

struct Check_IncDecline_Gender_Age
{
	unsigned short int incDecline;
	unsigned short int gender;
	unsigned short int ageCat;

	Check_IncDecline_Gender_Age(unsigned short int inIncDecline, unsigned short int inGender, unsigned short int inAgeCat) : incDecline(inIncDecline), gender(inGender), ageCat(inAgeCat){}

	bool operator() (Agent &a) {
		return(a.getIncomeDeclineT0() == incDecline && a.getGender()== gender && a.getAgeCat() == ageCat);
	}

};

struct Check_IncDecline_Gender_Age_PTSD
{
	unsigned short int incDecline;
	unsigned short int gender;
	unsigned short int ageCat;
	unsigned short int PTSDstatus;

	Check_IncDecline_Gender_Age_PTSD(unsigned short int inIncDecline, unsigned short int inGender, unsigned short int inAgeCat, unsigned short int inPTSDstatus ) : 
		incDecline(inIncDecline), gender(inGender), ageCat(inAgeCat), PTSDstatus(inPTSDstatus){}

	bool operator() (Agent &a) {
		return(a.getIncomeDeclineT0() == incDecline && a.getGender()== gender && a.getAgeCat() == ageCat && a.getPTSDstatus() == PTSDstatus);
	}

};

struct Check_LeftHome_Gender_Age
{
	unsigned short int leftHome;
	unsigned short int gender;
	unsigned short int ageCat;

	Check_LeftHome_Gender_Age(unsigned short int inLeftHome, unsigned short int inGender, unsigned short int inAgeCat) : leftHome(inLeftHome), gender(inGender), ageCat(inAgeCat){}

	bool operator() (Agent &a) {
		return(a.getLeftHomeT0() == leftHome && a.getGender()== gender && a.getAgeCat() == ageCat);
	}

};

struct Check_LeftHome_Gender_Age_PTSD
{
	unsigned short int leftHome;
	unsigned short int gender;
	unsigned short int ageCat;
	unsigned short int PTSDstatus;

	Check_LeftHome_Gender_Age_PTSD(unsigned short int inLeftHome, unsigned short int inGender, unsigned short int inAgeCat, unsigned short int inPTSDstatus) : 
	leftHome(inLeftHome), gender(inGender), ageCat(inAgeCat), PTSDstatus(inPTSDstatus) {}

	bool operator() (Agent &a) {
		return(a.getLeftHomeT0() == leftHome && a.getGender()== gender && a.getAgeCat() == ageCat && a.getPTSDstatus() == PTSDstatus);
	}

};


#endif STATISTICS_H