#include"stdafx.h"
#pragma once
#include "CPR.h"
#include<Controler.h>
#define DEBUG 1
CPR::CPR(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	setCentralWidget(ui.tabWidget);
	ui.posiPath->setText("D:/VS/CPR/plates");
	ui.negaPath->setText("D:/VS/CPR/noplates");
	ui.saveModelPath->setText("D:/VS/CPR/model");
	ui.chooseModelPath->setText("D:/VS/CPR/model");
	ui.testSamplePath->setText("D:/VS/CPR/testplate");
	//ui.tipLabel->setText("请");
	ui.tipLabel->setVisible(false);
	//ui.imgLabel->setPixmap();
	slotShowCurrCount(0);
	slotShowTotalCount(0);


	connect(ui.readImgBtn, SIGNAL(clicked()), this, SLOT(slotChooseDirBtn()));
	connect(ui.nextBtn, SIGNAL(clicked()), this, SLOT(slotNextBtn()));
	connect(ui.batchTestBtn, SIGNAL(clicked()), this, SLOT(slotBatchTestBtn()));
	connect(ui.startTrainBtn, SIGNAL(clicked()), this, SLOT(slotStartTrainBtn()));
	connect(ui.modelTestBtn, SIGNAL(clicked()), this, SLOT(slotModelTestBtn()));
	connect(ui.chooseImgBtn, SIGNAL(clicked()), this,SLOT(slotChooseImgBtn()));

	
}


void CPR::slotShowTotalCount(int totalCount) {
	QString title = "图片总数:  " + QString::number(totalCount)+"张";
	qDebug() << title;
	ui.totalCount->setText(title);
}
void CPR::slotShowCurrCount(int currCount) {
	QString title = "当前图片:  第" + QString::number(currCount) + "张";
	qDebug() << title;
	ui.currCount->setText(title);
}


//选取图片按钮被点击
void CPR::slotChooseImgBtn() {
	qDebug() << "slotChooseImgBtn";
	QStringList imgNames =QFileDialog::getOpenFileNames(this, "请选择单张或多张车牌图片","","Images (*.png *.jpg)");
	if (imgNames.empty()) {
		QMessageBox messageBox;
		messageBox.setWindowTitle("图片未选中");
		messageBox.setText("请重新选择图片");
		messageBox.exec();
		return;
	}
	emit chooseImgBtnClicked(imgNames);
}


void CPR::slotChooseDirBtn() {
	
	QString imgDirectory;
	imgDirectory = QFileDialog::getExistingDirectory(this, trUtf8("请选择车牌图片文件夹 ")); //由于编码问题，多了一个空格
	if (imgDirectory.isEmpty()) {
		QMessageBox messageBox;
		messageBox.setWindowTitle("文件夹未选中");
		messageBox.setText("请重新选择图片文件夹 "); //由于编码问题，多了一个空格
		messageBox.exec();
		return;
	}
	
	emit readImgBtnClicked(imgDirectory);
	//emit readImgBtnClicked(chooseDirectory(QString(""),"图片"));
}


void CPR::slotNextBtn() {
	
	qDebug() << "slotNextBtn";
	emit nextBtnClicked();
}

//先选择文件夹，再给Controler发送批量处理信号
void CPR::slotBatchTestBtn() {
	#ifdef DEBUG
		if (1)qDebug() << "slotBatchTestBtn";
	#endif // DEBUG
		slotChooseDirBtn();
	emit batchTestBtnClicked();
}

void CPR::slotStartTrainBtn() {
#ifdef DEBUG
	if (1)qDebug() << "slotStartTrainBtn";
#endif // DEBUG
	//QString posiPath = ui.posiPath->text();
	emit startTrainBtnClicked(ui.posiPath->text(), ui.negaPath->text(), ui.saveModelPath->text());
}
void CPR::slotModelTestBtn() {
	qDebug() << "slotModelTestBtn";
	ui.testSamplePath->text();
	emit modelTestBtnClicked(ui.testSamplePath->text());
}

void CPR::slotShowImg(const QImage &img) {
	#ifdef DEBUG
	if (1)qDebug() << "slotShowImg";
	#endif // DEBUG

	/*Mat类型转QImage
	std::string path2 = "D:/VS/CPR/CPR/images/A29922.jpg";
	cv::Mat cvimg = cv::imread(path2);
	QImage qimg;
	cv::cvtColor(cvimg, cvimg, cv::COLOR_BGR2RGB);
	qimg = QImage((const unsigned char*)(cvimg.data), cvimg.cols, cvimg.rows, QImage::Format_RGB888);
	*/

	ui.imgLabel->setPixmap(QPixmap::fromImage(img));
	
}
void CPR::slotShowMessage(const QString &message, const QString &title) {
	QMessageBox messageBox;
	if(title!="")messageBox.setWindowTitle(title);
	messageBox.setText(message); 
	messageBox.exec();
}

QString CPR::chooseDirectory(const QString &directory,const QString &dirType) {
	
	QString imgDirectory;
	imgDirectory = QFileDialog::getExistingDirectory(this, "请选择" + dirType+"文件夹 ",directory); //由于编码问题，多了一个空格
	if (imgDirectory.isEmpty()) {
		QMessageBox messageBox;
		messageBox.setWindowTitle(dirType+"文件夹未选中");
		messageBox.setText("请重新选择"+dirType+"文件夹 "); //由于编码问题，多了一个空格
		messageBox.exec();
		return directory;
	}
	else
	{
		return imgDirectory;
	}
}




void CPR::on_posPathBtn_clicked() {
	qDebug() << "on_posPathBtn_clicked";
	if (ui.posiPath->text() != "") {
		ui.posiPath->setText(chooseDirectory(ui.posiPath->text()));
	}
	
}

void CPR::on_negaPathBtn_clicked() {
	qDebug() << "on_negaPathBtn_clicked";
	if (ui.negaPath->text() != "") {
		ui.negaPath->setText(chooseDirectory(ui.negaPath->text()));
	}
	

}
void CPR::on_saveModelPathBtn_clicked() {
	qDebug() << "on_saveModelPathBtn_clicked";
	if (ui.saveModelPath->text() != "") {
		ui.saveModelPath->setText(chooseDirectory(ui.saveModelPath->text()));
	}
	
}
void CPR::on_chooseModelBtn_clicked() {
	qDebug() << "on_chooseModelBtn_clicked";
	if (ui.chooseModelPath->text() != "") {
		ui.chooseModelPath->setText(chooseDirectory(ui.chooseModelPath->text()));
	}
	
}
void CPR::on_testSamplePathBtn_clicked() {
	qDebug() << "on_testPathBtn_clicked";
	if (ui.testSamplePath->text() != "") {
		ui.testSamplePath->setText(chooseDirectory(ui.testSamplePath->text()));
	}	
}
