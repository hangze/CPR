#include"stdafx.h"
#pragma once
#include "Controler.h"
#include"PlateLocate.h"
#include<qtextcodec.h>
#define DEBUG
//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
Controler::Controler(QObject *parent)
	: QObject(parent)
{
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
	if (imgTotalCount> 0 && currImgCount< imgTotalCount&&currImgCount>=0) {//有图片未处理
		
		QString path = *imgDirectory +imgNameList[currImgCount];
		currImgCount++;

		std::string path1 = path.toLocal8Bit();
		PlateLocate::startPR(path.toStdString(), currPlate);
	}
	
	else {//无图片可处理
		emit
	}
}
void Controler::slotBatchTestBtn() {


}
void Controler::imgReadFailure() {
	//
}


CPlate::CPlate()
{
}

CPlate::~CPlate()
{
}