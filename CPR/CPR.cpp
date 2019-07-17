#include "CPR.h"
#include<qgridlayout.h>
#include<qdebug.h>
#include<iostream>
CPR::CPR(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setCentralWidget(ui.tabWidget);
	
	
	connect(ui.readImgBtn,SIGNAL(clicked()), SLOT(slotReadImg()));
	connect(ui.nextBtn, SIGNAL(clicked()), SLOT(slotNextBtn()));
	connect(ui.batchTestBtn, SIGNAL(clicked()), SLOT(slotBatchTestBtn()));
	
}
bool CPR::slotReadImg() {
	//读取图片列表并开始处理
	return true;
}
void CPR::slotNextBtn() {
	//处理下一张图片
}
void CPR::slotBatchTestBtn() {
	//开始批量测试
}
