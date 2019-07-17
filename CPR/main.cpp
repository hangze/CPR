#include "CPR.h"
#include <QtWidgets/QApplication>
#include<stdafx.h>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CPR w;	
	qApp->setStyle(QStyleFactory::create("fusion"));
	w.setWindowOpacity(0.9);
	w.show();
	return a.exec();
}
