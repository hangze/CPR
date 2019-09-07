#include"stdafx.h"
#pragma once
#include "Controler.h"
#include"PlateLocate.h"
#include<qtextcodec.h>
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
	PR = new PlateLocate();

}

Controler::~Controler()
{
}
void Controler::slotReadImgList( QString *Directory) {
	
	#ifdef DEBUG
		qDebug() << "slotReadImgList";
	#endif // DEBUG

	//获取文件名列表
	*imgDirectory = *Directory+"/";
	QDir dir(*imgDirectory);
	QStringList nameFiters;
	nameFiters << "*.jpg" << "*.png";
	imgNameList = dir.entryList(nameFiters, QDir::Files | QDir::Readable, QDir::Name);
	setCurrImgCount(0);
	setImgTotalCount(imgNameList.size());
	
}

void Controler::slotProcessImg() {
	if (imgTotalCount <= 0 || currImgCount >=imgTotalCount||currImgCount<0) {//无图片可处理
		qDebug() << "no image";
		emit signalShowMessage(u8"所选文件夹无图片或图片已处理完毕");
	}
	
	else {//有图片未处理
		
		QString path = *imgDirectory + imgNameList[currImgCount];
		currImgCount++;

		std::string stdpath = path.toLocal8Bit();//转化为本地字符，与源文件编码方式相关
		PR->startPR(stdpath);
		
	}
	
}
void Controler::showImg(const cv::Mat &img) {
	
	qDebug() << "showImg";
	cv::Mat tempImg;
	cv::cvtColor(img, tempImg, cv::COLOR_BGR2RGB);
	//cv::resize(img, img, cv::Size(1280, 720));
	//static QImage tempImg2;
	tempImg2= QImage((const unsigned char *)(tempImg.data), tempImg.cols, tempImg.rows, QImage::Format_RGB888);
	emit signalShowImg(tempImg2); 
}
//void Controler::showPlateImg(const cv::Mat &img) {
//
//}

//文件夹选择完毕，开始批量测试
void Controler::slotBatchTest() {
	#ifdef DEBUG
		if (1)qDebug() << "slotBatchTest";
	#endif // DEBUG

		clock_t start, finish;
		

		while (imgTotalCount > 0 && currImgCount < imgTotalCount&&currImgCount >= 0)
		{
			QString  path = *imgDirectory + imgNameList[currImgCount];
			
			//std::string directory = (*imgDirectory).toLocal8Bit();
			//std::string imgName = imgNameList[currImgCount].toLocal8Bit();
			currImgCount++;

			std::string stdpath = path.toLocal8Bit();//转化为本地字符，与源文件编码方式相关
			
			start = clock();//计算时间
			bool isBatchTest = true;
			PR->startPR(stdpath, isBatchTest);

			finish = clock();
			totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
			qDebug() << "this function running time is " << totaltime << "s!";
			alltime += totaltime;
			totaltime = 0;

		}
		//PR->xMainNode.writeToFile(resultPath.c_str());
		(*PR).xMainNode.writeToFile((*PR).resultPath.c_str());
		qDebug() << "averge  running time is " << alltime/ imgTotalCount << "s!";

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
	
	if (svm->startJudge(srcImg)) { 
		plates++;
	}
	else {
		showImg(srcImg);
	}
	}
	qDebug() <<u8"正确率="<< float(plates) / posImgNameList.size();
}




CPlate::CPlate()
{

}

CPlate::~CPlate()
{

}