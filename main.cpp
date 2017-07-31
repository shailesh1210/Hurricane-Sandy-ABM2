//#include <QtWidgets/QApplication>
#include "skeleton.h"
#include "Agent.h"
#include "FileHandling.h"
#include "Parameters.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QRect rec = QApplication::desktop()->screenGeometry();
	Skeleton w;
	w.resize(rec.width(), rec.height()-75);
	//w.setFixedSize(2000, 1000);
	w.setWindowTitle("Sandy ABM");
	w.show();
	return a.exec();
}
