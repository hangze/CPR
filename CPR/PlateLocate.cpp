#include"stdafx.h"
#pragma once
#include "PlateLocate.h"
#include "Controler.h"
#define DEBUG

#define SHOW_IMAGE(imgName, debug) \
  if (debug) { \
    namedWindow("imgName", WINDOW_AUTOSIZE); \
    moveWindow("imgName", 500, 500); \
    imshow("imgName", imgName); \
    waitKey(0); \
    destroyWindow("imgName"); \
  }

PlateLocate::PlateLocate()
{
}


PlateLocate::~PlateLocate()
{
}



/**
 * @brief 开始车牌识别 
 * 车牌识别的入口
 * @param path     车牌图像的路径
 * @param plate     车牌信息结果
 *
 * @return 是否定位成功
 *     -<em>false</em> 定位成功
 *     -<em>true</em> 定位失败
 */
bool PlateLocate::startPR(std::string &path, CPlate &plate) {
	
	#ifdef DEBUG
		qDebug() << "startPR";
	#endif // DEBUG

	cv::Mat srcImg = cv::imread(path);
	if (!srcImg.data) {
		Controler::imgReadFailure(); 
		return false;
	}
	//亮度均衡
	cv::Mat valueEqualImg;
	cv::cvtColor(srcImg, valueEqualImg, cv::COLOR_BGR2HSV);
	std::vector<cv::Mat> valueEqualImg3;
	cv::split(valueEqualImg, valueEqualImg3);
	cv::equalizeHist(valueEqualImg3[2], valueEqualImg3[2]);
	cv::merge(valueEqualImg3, valueEqualImg);


	//显示效果
	#ifdef DEBUG
	if (1) {
		cv::Mat tempImg;
		cv::cvtColor(srcImg, tempImg, cv::COLOR_BGR2RGB);
		QImage tempImg2 = QImage((const unsigned char *)tempImg.data, tempImg.cols, tempImg.rows, QImage::Format_RGB888);

		}
	#endif // DEBUG
	return true;
}