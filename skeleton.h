#ifndef SKELETON_H
#define SKELETON_H

#include <QWidget>
#include <QDialog> 
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QFormLayout>
#include <QTimer>
#include "qcustomplot.h"

//#include "ui_skeleton.h"
#include "Parameters.h"

class Agent;
class FileHandling;
class Statistics;
class QGroupBox;
class QCheckBox;

class Skeleton : public QWidget
{
	Q_OBJECT

public:
	Skeleton(QWidget *parent = 0);
	virtual ~Skeleton();

//private:
//	Ui::SkeletonClass ui;

private slots:
	void initialize();
	void run();
	void reset();
	void stats();
	void enableGroupBoxSC(int state);
	void enableGroupBoxUC(int state);
	void realtimeDataSlot();

private:
	//QListWidget *list;
	QGroupBox *createFirstGroupBox();
	QGroupBox *createSecondGroupBox();
	QGroupBox *createThirdGroupBox();
	QGroupBox *createFourthGroupBox();
	QGroupBox *createTrtmentSubGroupBox(QGroupBox *grpBox, QComboBox *combo1, QComboBox *combo2, const QString boxTitle, const QString c1Label, QStringList c1Items , const QString c2Label, QStringList c2Items);
	QGroupBox *createResourcesSubGroupBox(QGroupBox *grpBox, QLineEdit *mealServed, const QString boxTitle);
	//group box for input parameter
	
	QGroupBox *inputParamGrpBox;
	QLineEdit *numAgentsInput;
	QLineEdit *numStepsInput;
	QLineEdit *numTrialsInput;

	QLineEdit *numBoroInput;
	QLineEdit *numAgeCatInput;
	QLineEdit *numRaceCatInput;
	QLineEdit *numIncCatInput;
	QLineEdit *numSubIncCatInput;
	
	//group box for treatment type (Stepped care and Usual Care)
	QGroupBox *trtmentGrpBox;
	QCheckBox *steppedCareCB;
	QGroupBox *costGrpBoxSC;
	QComboBox *costComboCBT;
	QComboBox *costComboSPR;
	QGroupBox *sessionGrpBoxSC;
	QComboBox *sessionComboCBT;
	QComboBox *sessionComboSPR;

	QCheckBox *usualCareCB;
	QGroupBox *costGrpBoxUC;
	QComboBox *costComboSPR1;
	QGroupBox *sessionGrpBoxUC;
	QComboBox *sessionComboSPR1;

	//group box for push buttons
	QGroupBox *pushBtnGrpBox;
	QPushButton *_initialize;
	QPushButton *_run;
	QPushButton *_reset;
	QPushButton *_showStats;
	//QPushButton *removeAll;
	//QDialog *demoStatsDialog;
	//QTextEdit *statsViewer;

	//group box for environment variables
	QGroupBox *resourcesGrpBox;
	QLineEdit *mealPerDay;
	QLineEdit *durMealDist;

	QGroupBox *bronxGrpBox;
	QLineEdit *bronxMealServed;

	QGroupBox *brookGrpBox;
	QLineEdit *brookMealServed;

	QGroupBox *manhGrpBox;
	QLineEdit *manhMealServed;

	QGroupBox *queensGrpBox;
	QLineEdit *queensMealServed;

	QGroupBox *statenGrpBox;
	QLineEdit *statenMealServed;

	QCustomPlot *customPlot;
	QTimer dataTimer;

	double timeCounter;

protected:
	void createAgents();
	void createHousehold();

	void createHHSize1(Agent *agent, int &index); // creates a list of households with size 1
	void createHHSize234Step1(int &index); // creates a list of households with size 2, 3, 4 based on race
	void createHHSize234Step2(int &index); // creates a list of households with size 2, 3 and 4 irrespective of race

	void mergeHouseholds();

protected:
	void fillIncompleteHouseholds();
	void addUnmatchedAgents(int &index);

protected:
	void resetCounter();

protected:

	AgentMap _tempAgentsMap;
	AgentList _agentListFinal;

	AgentList _testAgent;

	FamilyList _famList;
	FamilyList _famListHHsize2;
	
	int _numAgents;
	int _numSteps;
	int _numTrials;

	int _numBoro;
	int _ageCat;
	int _raceCat;
	int _incCat;
	int _subIncCat;

	

	int _mealInventoryBronx;
	int _numPplServedBronx;

	int _mealInventoryBrook;
	int _numPplServedBrook;

	int _mealInventoryManh;
	int _numPplServedManh;

	int _mealInventoryQueens;
	int _numPplServedQueens;

	int _mealInventoryStaten;
	int _numPplServedStaten;

	bool _steppedCare;
	int _costCBTtrtment;
	int _costSPRtrtment;
	int _sessionCBT;
	int _sessionSPR;

	bool _usualCare;
	int _costSPR1trtment;
	int _sessionSPR1;

public:

	std::vector<int> _mealInventory;
	std::vector<int> _totalPplServed;
	std::vector<int> _countPplServed;

	int _mealPerDay;
	int _durMealDist;
};

#endif SKELETON_H
