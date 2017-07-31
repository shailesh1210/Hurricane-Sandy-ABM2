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
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>

//#include "ui_skeleton.h"
#include "Parameters.h"

class Agent;
class FileHandling;
class Statistics;
class QGroupBox;
class QCheckBox;

#define NUM_PTSD_PLOTS 2
#define NUM_PTSD_SUB_PLOTS 6
#define NUM_STATE_SUB_PLOTS 8

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
	void enableSubGroupBoxTrtment();
	void realtimeDataSlot();

private:
	FileHandling *file;
	
	//QListWidget *list;
	QGroupBox *createFirstGroupBox();
	QGroupBox *createSecondGroupBox();
	QGroupBox *createThirdGroupBox();
	QGroupBox *createFourthGroupBox();
	QGroupBox *createTrtmentSubGroupBox(QGroupBox *grpBox, const QString boxTitle, const int flag);

	QCustomPlot *createPTSDPlot(const QString titleName);
	QCustomPlot *createStatePlot(const QString titleName);
	//group box for input parameter
	
	//first group box
	QGroupBox *inputParamGrpBox;

	QLineEdit *numAgentsInput;
	QLineEdit *numStepsInput;
	QLineEdit *numTrialsInput;


	QLineEdit *numBoroInput;
	QLineEdit *numAgeCatInput;
	QLineEdit *numRaceCatInput;
	QLineEdit *numIncCatInput;
	QLineEdit *numSubIncCatInput;

	//second group box
	QGroupBox *screeningGrpBox;

	QLineEdit *sensitivityInput;
	QLineEdit *specificityInput;
	QLineEdit *screenStartTimeInput;
	QLineEdit *ptsdCutOffInput;
	
	//third group box
	QGroupBox *trtmentGrpBox;

	QGroupBox *trtmentTypeGrpBox;
	QCheckBox *steppedCareCB;
	QCheckBox *usualCareCB;

	QGroupBox *trtmentTimeGrpBox;
	QLineEdit *trtmentTimeInput;
	QLineEdit *socialWorkerTimeInput;
	QLineEdit *trtmentWaitTimeInput;

	QGroupBox *trtmentStrengthGrpBox;
	QLineEdit *cbtResolutionCoffInput;
	QLineEdit *sprResolutionCoffInput;
	QLineEdit *naturalDecayCoffInput;

	QGroupBox *costGrpBox;
	QComboBox *costComboCBT;
	QComboBox *costComboSPR;

	QGroupBox *sessionGrpBox;
	QComboBox *sessionComboCBT;
	QComboBox *sessionComboSPR;

	//fourth group box
	QGroupBox *pushBtnGrpBox;
	QPushButton *initializePb;
	QPushButton *runPb;
	QPushButton *resetPb;
	QPushButton *showStatsPb;
	
	//fifth group box
	QGroupBox *relapseGrpBox;
	QLineEdit *relapsePTSDxInput;
	QLineEdit *relapseTimeInput;
	QLineEdit *numRelapseInput;
	QLineEdit *propRelapseInput;
	
	//QCustomPlot *ptsdTrtmentPlot;
	QCustomPlot *ptsdPlot[NUM_PTSD_PLOTS];
	/*QCustomPlot *ptsdPlot1;
	QCustomPlot *ptsdPlot2;
	QCustomPlot *ptsdPlot3;
	QCustomPlot *ptsdPlot4;*/
	QCustomPlot *statePlot1;
	QCustomPlot *statePlot2;

	//QCustomPlot *statePlot;

	QVector<int> red;
	QVector<int> blue;
	QVector<int> green;

	QTimer dataTimer;

	unsigned short int timeCounter;

protected:
	void setInputParameters();
	void setTreatmentParameters();
	void setRelapseParameters();
	void createAgents();
	void createHousehold();

	void createHHSize1(Agent *agent, unsigned int &index); // creates a list of households with size 1
	void createHHSize234Step1(unsigned int &index); // creates a list of households with size 2, 3, 4 based on race
	void createHHSize234Step2(unsigned int &index); // creates a list of households with size 2, 3 and 4 irrespective of race

	void mergeHouseholds();
	void fillIncompleteHouseholds();
	void addUnmatchedAgents(unsigned int &index);
	void updatePlots();

//protected:
//	void resetCounter();

protected:

	AgentMap tempAgentsMap;
	AgentList agentListFinal;

	//AgentList _testAgent;

	FamilyList famList;
	FamilyList famListHHsize2;
	
	unsigned int numAgents;
	unsigned short int numSteps;
	unsigned short int numTrials;

	unsigned short int numBoro;
	unsigned short int ageCat;
	unsigned short int raceCat;
	unsigned short int incCat;
	unsigned short int subIncCat;

public:

	float sensitivity;
	float specificity;
	unsigned short int ptsdScreeningTime;
	float ptsdxCutoff;

	unsigned short int trtmentTime;
	unsigned short int trtmentWaitTime;
	unsigned short int socialWorkerTime;

	float cbtCoeff;
	float sprCoeff;
	float natDecayCoeff;

	bool steppedCare;
	unsigned short int costCBTtrtment;
	unsigned short int costSPRtrtment;
	unsigned short int maxSessionsCBT;
	unsigned short int maxSessionsSPR;

	bool usualCare;

	boost::container::vector<float>potency;

	float relapsePTSDx;
	unsigned short int relapseTime;
	unsigned short int numRelapse;

	boost::container::vector<int>countCBT;
	boost::container::vector<int>countSPR;
	boost::container::vector<int>countND;

	int countExpedite;
	int countNormal;
	int countNormalNoSW;
	int countNormalUC;
};



#endif SKELETON_H
