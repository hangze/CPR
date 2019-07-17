#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CPR.h"

class CPR : public QMainWindow
{
	Q_OBJECT

public:
	CPR(QWidget *parent = Q_NULLPTR);

private:
	Ui::CPRClass ui;
private slots:
	bool slotReadImg();
	void slotNextBtn();
	void slotBatchTestBtn();
};
