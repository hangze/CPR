#include"stdafx.h"
#pragma once
#include "Controler.h"
#include"PlateLocate.h"
#include"SVMJudge.h"

#define DEBUG

//获得静态成员，用于自动析构单例

//Controler::CGarbo Controler::Garbo = new Controler::CGarbo();

//手动销毁单例，地址为空返回false
bool Controler::destroy() {
	if (nullptr != controler) { 
		delete controler;
		controler = nullptr;
		return true;
	}
	return false;
}


//获得单例	
Controler* Controler::getControler(QObject *parent ) {
	if (nullptr == controler) {
		controler = new Controler(parent);

	}
	return controler;
}
Controler* Controler::controler = nullptr;
//QImage Controler::tempImg2= new QImage();

 
Controler::Controler(QObject *parent)
	: QObject(parent)
{
	currImgCount = 0;
	imgTotalCount = 0;
	//PR = new PlateLocate();

}

Controler::~Controler()
{
}
//由路径得到图片文件名列表
void Controler::slotReadImgList( QString Directory) {
	
	#ifdef DEBUG
		qDebug() << "slotReadImgList";
	#endif // DEBUG

	//获取文件名列表
	QString imgDirectory = Directory+"/";
	QDir dir(imgDirectory);
	if (!dir.exists())
		qWarning("Cannot find the example directory");

	QStringList nameFiters;
	nameFiters << "*.jpg" << "*.png";
	QStringList nameList;
	
	nameList = dir.entryList(nameFiters, QDir::Files | QDir::Readable, QDir::Name);
	std::string directory=imgDirectory.toStdString();
	//更新图片路径表
	vecImgPath.clear();
	for (int i = 0; i < nameList.size(); i++) {
		std::string imgPathName = directory + nameList[i].toStdString();
		vecImgPath.push_back(imgPathName);
	}
	
	setCurrImgCount(0);
	setImgTotalCount(nameList.size());

}

//得到图片文件名列表
void Controler::slotReadImgList(const QStringList &PathList) {
	qDebug() << "slotReadImgList";
	vecImgPath.clear();
	for (int i = 0; i < PathList.size(); i++) {
		std::string imgPath = PathList[i].toStdString();
		vecImgPath.push_back(imgPath);
	}
	setCurrImgCount(0);
	setImgTotalCount(PathList.size());
	//imgPathList
}

void Controler::slotProcessImg() {
	if (imgTotalCount <= 0 || currImgCount >=imgTotalCount||currImgCount<0) {//无图片可处理
		qDebug() << "no image";
		emit signalShowMessage(u8"所选文件夹无图片或图片已处理完毕");
	}
	
	else {//有图片未处理
		
		std::string stdpath = vecImgPath[currImgCount];
		currImgCount++;
		PRFlow.startPR(stdpath);		
	}
	
}
void Controler::showImg(const cv::Mat &img) {
	
	qDebug() << "showImg";
	cv::Mat tempImg;
	cv::cvtColor(img, tempImg, cv::COLOR_BGR2RGB);
	//cv::resize(img, img, cv::Size(1280, 720));
	//static QImage tempImg2;
	plateImg= QImage((const unsigned char *)(tempImg.data), tempImg.cols, tempImg.rows, QImage::Format_RGB888);
	emit signalShowImg(plateImg);
}
//void Controler::showPlateImg(const cv::Mat &img) {
//
//}

//文件夹选择完毕，开始批量测试
void Controler::slotBatchTest() {
	#ifdef DEBUG
		if (1)qDebug() << "slotBatchTest";
	#endif // DEBUG	
			PRFlow.batchTest(vecImgPath);
}

void Controler::slotStartTrain(const QString &posiPath, const QString &negaPath, const QString &saveModelPath) {
	
	SVMJudge *svm = new SVMJudge();
	svm->startTrain(posiPath.toStdString(),negaPath.toStdString(),saveModelPath.toStdString(),cv::Size(136,36));
	qDebug() << "slotStartTrain";
}

void Controler::slotModelTest(const QString &testSamplePath) {
	
	qDebug() << "slotModelTest";
	QDir dir(testSamplePath);
	QStringList nameFiters;
	nameFiters << "*.jpg" << "*.png";
	QStringList posImgNameList = dir.entryList(nameFiters, QDir::Files | QDir::Readable, QDir::Name);
	int plates=0;
	SVMJudge *svm = new SVMJudge();
	for(int i=0;i<posImgNameList.size();i++){
	cv::Mat srcImg = cv::imread(testSamplePath.toStdString()+"/"+posImgNameList[i].toStdString());
	float score;
	if (svm->startJudge(srcImg,score)) { 
		plates++;
	}
	else {
		showImg(srcImg);
	}
	}
	qDebug() <<u8"正确率="<< float(plates) / posImgNameList.size();
}

void Controler::setPlateImg(const cv::Mat &img) {
	qDebug() << "setPlateImg";
	cv::Mat tempPlateImg;
	cv::cvtColor(img, tempPlateImg, cv::COLOR_BGR2RGB);
	//cv::resize(img, img, cv::Size(1280, 720));
	//static QImage tempImg2;
	plateImg = QImage((const unsigned char *)(tempPlateImg.data), tempPlateImg.cols, tempPlateImg.rows, QImage::Format_RGB888);
	emit signalShowImg(plateImg);
}
