#include"stdafx.h"
#pragma once
#include "PlateLocate.h"
#include "Controler.h"
#include<util.h>
#include"SVMJudge.h"
//extern Controler *controler;
#define DEBUG 1
//macro config
#define IDEA_WIDTH 800
#define IDEA_HIGHT 600
#define DEAFAULT_GAUSIIBLUR_SIZE 5
#define DEAFAULT_MORPH_WIDTH 17
#define DEAFAULT_MORPH_HIGHT 3

#define DEAFAULT_SIZE_ERROR 0.4
#define DEAFAULT_SIZE_REFINE_ERROR 0.2//精定位时的判别偏差
#define DEAFAULT_SIZE_ASPECT 3.75//车牌大小默认长宽比，注意，与图纸数据比例3.17不同
#define DEAFAULT_SIZE_MINAREA 900 //
#define DEAFAULT_SIZE_MAX_AREA 34000 //目前的值由少量样本目测得来，注意，大小与图片大小(现长为800，比例固定)息息相关
#define DEAFAULT_TEMPIMG "D:\\VS\\CPR\\TEMPIMG\\"
PlateLocate::PlateLocate()
{
}


PlateLocate::~PlateLocate()
{
}
int PlateLocate::platecout = 0;
std::string PlateLocate::tempPath = "D:/VS/CPR/recoPlate/";

/**
 * @brief 开始车牌识别 
 * 车牌识别的入口
 * @param path      车牌图像的路径
 * @param plate     车牌信息结果
 *
 * @return 是否定位成功
 *     -<em>false</em> 定位成功
 *     -<em>true</em> 定位失败
 */
bool PlateLocate::startPR(std::string &path, CPlate &currPlates) {
	
	#ifdef DEBUG
		qDebug() << "startPR";
	#endif // DEBUG

	cv::Mat srcImg = cv::imread(path);
	if (!srcImg.data) {
		Controler::getControler()->ShowMessage("图片读取失败");
		return false;
	}
	
	//cv::Mat tempImg=srcImg.clone();//测试能否删除
	cv::Mat tempImg;
	preProcess(srcImg, tempImg);	
	Controler::getControler()->showImg(tempImg);
	std::vector<cv::RotatedRect> rotatedRects;
	qDebug() << rotatedRects.size();
	sobelResearch(srcImg, rotatedRects);//改动
	if (rotatedRects.size() <= 0) {
		return false;//定位失败，后期可设置特定标志位
	}
	plateJudge(srcImg, rotatedRects, currPlates);
	#ifdef DEBUG	
		qDebug() << "paltecout=" << platecout;
	#endif // DEBUG
	


	//显示效果
	#ifdef DEBUG
	if (0) {
		Controler::getControler()->showImg(srcImg);
		}
	#endif // DEBUG
	return true;
}

void PlateLocate::preProcess(const cv::Mat &preImg, cv::Mat &outImg) {
	//图像预处理
	int newrows = preImg.rows*IDEA_WIDTH / preImg.cols;
	cv::resize(preImg, outImg, cv::Size(IDEA_WIDTH, newrows));	
	cv::GaussianBlur(outImg, outImg, cv::Size(DEAFAULT_GAUSIIBLUR_SIZE, DEAFAULT_GAUSIIBLUR_SIZE), 0, 0, cv::BORDER_DEFAULT);	
	//getImgPoint(outImg, "test");

	/*
	#ifdef DEBUG
		SHOW_IMAGE(outImg, 1);
	#endif // DEBUG
	*/
}


/**
 * @brief 判断矩形图片是否车牌，是车牌则存入CPlate中 
 * detail description
 * @param srcImg    原始整图,使用未经处理的原始图，效果更佳
 * @param rotatedRects    可能为车牌的旋转矩形
 * @param currPlate    当前的CPlate，用于存储车牌信息
 */
void PlateLocate::plateJudge(const cv::Mat &srcImg,std::vector<cv::RotatedRect> &rotatedRects,CPlate&currPlates) {
	
	SVMJudge svmJudge;
	cv::Mat plateImg;
	for (int i = 0; i < rotatedRects.size(); i++) {
		
		util::getRotatedRectArea(srcImg, rotatedRects[i], plateImg);
		
		if (svmJudge.startJudge(plateImg)) {
			currPlates.plateImgs.push_back(plateImg);
			currPlates.rRects.push_back(rotatedRects[i]);
			platecout++;
			cv::imwrite(tempPath+std::to_string(Controler::getControler()->getCurrImgCount())+ std::to_string(i)+".jpg", plateImg);
			//Controler::getControler()->showImg(plateImg);
		}
		
	}
}
void PlateLocate::sobelResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect> &rRects) {
	//sobel查找车牌
	
	cv::Mat tempImg2 ;
	cv::Mat tempImg;
	preProcess(inputImg, tempImg2);//增加
	cv::cvtColor(tempImg2, tempImg, cv::COLOR_BGR2GRAY);//改动

	cv::Sobel(tempImg, tempImg, CV_8U, 1, 0);
	cv::Mat element= cv::getStructuringElement(cv::MORPH_RECT, cv::Size(DEAFAULT_MORPH_WIDTH, DEAFAULT_MORPH_HIGHT));
	cv::Mat sobelpri ;
	
	cv::morphologyEx(tempImg, sobelpri, cv::MORPH_CLOSE,element);
	cv::threshold(sobelpri, sobelpri, 255, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);

	std::vector<std::vector<cv::Point>> contours;
	cv::RotatedRect rotatedRect;
	cv::findContours(sobelpri, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator it= contours.begin();	
	for (int i=0; it != contours.end(); it++,i++) {
		rotatedRect=cv::minAreaRect(cv::Mat(*it));		
		if(vertifySizes(rotatedRect,DEAFAULT_SIZE_ERROR)){
			
			cv::Point2f* vertices = new cv::Point2f[4];
			rotatedRect.points(vertices);
			cv::Mat rRectArea;
			util::getRotatedRectArea(inputImg, rotatedRect, rRectArea);//截取图像后期可换成截取未处理的原图
			#ifdef DEBUG    //画出符合条件的矩形框并存储图像，后期作车牌判断图像			
			std::string path = DEAFAULT_TEMPIMG + std::to_string(Controler::getControler()->getCurrImgCount()) + std::to_string(i) + ".jpg";
			qDebug() << cv::imwrite(path, rRectArea);//如果写入成功将输出true
			//画旋转矩形
			std::vector<cv::Point>rectcontour;
			for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
			std::vector<std::vector<cv::Point>> rectcontous;
			rectcontous.push_back(rectcontour);
			cv::drawContours(tempImg2, rectcontous, 0, cv::Scalar(255, 255, 0), 1);//改动
			#endif // DEBUG	
			rRects.push_back(rotatedRect);
		}
		
	}
	Controler::getControler()->showImg(tempImg2);
	//getImgPoint(inputImg, "sobel");
}


/**
 * @brief 判断矩形是否满足条件
 * @param RotatedRect    旋转矩形
 * @param error    长宽比偏差
 *
 * @return 是否满足条件
 *     -<em>false</em> 不满足条件
 *     -<em>true</em> 满足条件
 */
bool PlateLocate::vertifySizes(const cv::RotatedRect &rRect,float error) {
	float mError = error;//长宽比偏差值

	float ratioMin =DEAFAULT_SIZE_ASPECT-DEAFAULT_SIZE_ASPECT * mError;//计算得到最小比例
	float ratioMax= DEAFAULT_SIZE_ASPECT + DEAFAULT_SIZE_ASPECT * mError;//计算得到最大比例

	int areaMin = DEAFAULT_SIZE_MINAREA;
	int areaMax = DEAFAULT_SIZE_MAX_AREA;
	
	//计算得到候选矩形长宽比和面积
	float ratio = float(rRect.size.width )/ float(rRect.size.height);
	int area = rRect.size.width*rRect.size.height;
	if (ratio < 1) {
		ratio = 1 / ratio;
		if (ratio < ratioMin || ratio > ratioMax || area<areaMin || area > areaMax
			|| rRect.angle>-70)    
		{//尽量用or避免用and//||(rRect.angle<-30&&rRect.angle>-70)
			return false;
		}
	}
		else if (ratio < ratioMin || ratio > ratioMax || area<areaMin || area > areaMax
			|| rRect.angle<-30 ) {//尽量用or避免用and//||(rRect.angle<-30&&rRect.angle>-70)
			return false;
		}
	
	

	return true;
}