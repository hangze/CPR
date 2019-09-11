#include<stdafx.h>
#pragma once
#include "CPR.h"
#include"Controler.h"
#include <QMetaType>
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	CPR *w=new CPR();	
	qApp->setStyle(QStyleFactory::create("fusion"));
	w->setWindowOpacity(0.9);
	w->show();


	Controler *controler =Controler::getControler();
	QThread *thread1 = new QThread();
	controler->moveToThread(thread1);
	qRegisterMetaType<QImage>("QImage&");
	qRegisterMetaType<QImage>("QStringList&");
	qRegisterMetaType<QImage>("QImage");
	QObject::connect(w, SIGNAL(readImgBtnClicked( QString)), controler, SLOT(slotReadImgList(QString)),Qt::QueuedConnection);
	QObject::connect(w, SIGNAL(nextBtnClicked()), controler, SLOT(slotProcessImg()), Qt::QueuedConnection);
	QObject::connect(w, SIGNAL(batchTestBtnClicked()), controler, SLOT(slotBatchTest()), Qt::QueuedConnection);
	QObject::connect(w, SIGNAL(startTrainBtnClicked(const QString &, const QString &, const QString &)), controler, SLOT(slotStartTrain(const QString &, const QString &, const QString &)), Qt::QueuedConnection);
	QObject::connect(w, SIGNAL(modelTestBtnClicked(const QString &)), controler, SLOT(slotModelTest(const QString &)),Qt::QueuedConnection);
	QObject::connect(w, SIGNAL(chooseImgBtnClicked(const QStringList &)), controler, SLOT(slotReadImgList(const QStringList&)));

	QObject::connect(controler, SIGNAL(signalShowImg(const QImage&)), w, SLOT(slotShowImg(const QImage&)), Qt::BlockingQueuedConnection);
	QObject::connect(controler, SIGNAL(signalShowMessage(const QString&, const QString&)), w, SLOT(slotShowMessage(const QString &, const QString&)));
	QObject::connect(controler, SIGNAL(signalShowTotalCount(int)), w, SLOT(slotShowTotalCount(int)));
	QObject::connect(controler, SIGNAL(signalShowCurrCount(int)), w, SLOT(slotShowCurrCount(int)));
	thread1->start();

	return a.exec();
}
