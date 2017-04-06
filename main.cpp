//#include <QtWidgets/QApplication>
#include "skeleton.h"
#include "Agent.h"
#include "FileHandling.h"
#include "Parameters.h"


int main(int argc, char *argv[])
{
	// This is a test
	QApplication a(argc, argv);
	Skeleton w;
	//w.setFixedSize(240,280);
	w.setWindowTitle("Sandy ABM");
	w.show();
	return a.exec();
}
