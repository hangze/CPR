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



	connect(ui.readImgBtn, SIGNAL(clicked()), this, SLOT(slotReadImgBtn()));
	connect(ui.nextBtn, SIGNAL(clicked()), this, SLOT(slotNextBtn()));
	connect(ui.batchTestBtn, SIGNAL(clicked()), this, SLOT(slotBatchTestBtn()));
	connect(ui.startTrainBtn, SIGNAL(clicked()), this, SLOT(slotStartTrainBtn()));
	connect(ui.modelTestBtn, SIGNAL(clicked()), this, SLOT(slotModelTestBtn()));
	//connect( )
	
}

void CPR::slotReadImgBtn() {
	QString *imgDirectory=new QString;
	*imgDirectory = QFileDialog::getExistingDirectory(this, trUtf8("请选择车牌图片文件夹 ")); //由于编码问题，多了一个空格
	if (imgDirectory->isEmpty()) {
		QMessageBox messageBox;
		messageBox.setWindowTitle("文件夹未选中");
		messageBox.setText("请重新选择图片文件夹 "); //由于编码问题，多了一个空格
		messageBox.exec();
		return;
	}

	emit readImgBtnClicked(imgDirectory);
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
		slotReadImgBtn();
	emit batchTestClicked();
}

void CPR::slotStartTrainBtn() {
#ifdef DEBUG
	if (1)qDebug() << "slotStartTrainBtn";
#endif // DEBUG
	//QString posiPath = ui.posiPath->text();
	emit startTrainClicked(ui.posiPath->text(), ui.negaPath->text(), ui.saveModelPath->text());
}
void CPR::slotModelTestBtn() {
	qDebug() << "slotModelTestBtn";
	ui.testSamplePath->text();
	emit modelTestClicked(ui.testSamplePath->text());
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

	ui.label->setPixmap(QPixmap::fromImage(img));
	
}
void CPR::slotShowMessage(const QString &message, const QString &title) {
	QMessageBox messageBox;
	if(title!="")messageBox.setWindowTitle(title);
	messageBox.setText(message); 
	messageBox.exec();
}

QString CPR::chooseDirectory(const QString &directory) {
	
	QString *imgDirectory = new QString;
	*imgDirectory = QFileDialog::getExistingDirectory(this, trUtf8("请选择文件夹 "),directory); //由于编码问题，多了一个空格
	if (imgDirectory->isEmpty()) {
		QMessageBox messageBox;
		messageBox.setWindowTitle("文件夹未选中");
		messageBox.setText("请重新选择文件夹 "); //由于编码问题，多了一个空格
		messageBox.exec();
		return directory;
	}
	else
	{
		return *imgDirectory;
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
