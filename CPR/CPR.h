#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_CPR.h"
#include <QObject>
#include<qimage.h>
class CPR : public QMainWindow
{
	Q_OBJECT

public:
	CPR(QWidget *parent = Q_NULLPTR);
signals:
	void readImgBtnClicked(QString *directory);
	void nextBtnClicked();
	void batchTestClicked();
	void startTrainClicked(const QString &posiPath, const QString &negaPath, const QString &saveModelPath);
	void modelTestClicked(const QString &testSamplePath);
private:
	Ui::CPRClass ui;
	inline QString chooseDirectory(const QString &directory);
	
private slots:
	void slotReadImgBtn();
	void slotNextBtn();
	void slotBatchTestBtn();
	void slotStartTrainBtn();
	void slotModelTestBtn();
	void slotShowImg(const QImage &img);
	static void slotShowMessage(const QString &message, const QString &title = "");
	
	void on_posPathBtn_clicked();
	void on_negaPathBtn_clicked();
	void on_saveModelPathBtn_clicked();
	void on_chooseModelBtn_clicked();
	void on_testSamplePathBtn_clicked();

};
