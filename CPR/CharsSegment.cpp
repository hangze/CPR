#include "stdafx.h"
#include "CharsSegment.h"
#include"util.h"

void CharsSegment::genernalSegment(std::vector<CPlate> &vecCPlate) {
	for (int i = 0; i < vecCPlate.size(); i++) {
		cv::Mat plateImg = vecCPlate[i].plateImg.clone();
		cv::Mat plateBinaryImg;
		std::vector<cv::RotatedRect>charsRRect;
		segmentUsingProjection(plateImg, plateBinaryImg, charsRRect);
		vecCPlate[i].plateBinaryImg = plateBinaryImg;
		vecCPlate[i].characterRects = charsRRect;
	}
}


/**
 * @brief 投影法分割字符
 * detail description
 * @param plateImg    原车牌图像
 * @param outputThresImg    输出的车牌二值图像，后期用于识别
 */
void CharsSegment::segmentUsingProjection(cv::Mat &plateImg,cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr) {
	
	cv::Mat plateImgThres = plateImg.clone();
	cv::imshow("dddd", plateImgThres);
	//cv::waitKey(0);
	thresInterface(plateImgThres, plateImgThres);
		
	//clear edge
		//判断是否黄车牌，翻转灰度
	cv::Mat rectImg = cv::Mat(plateImgThres, cv::Rect(24, 0, 88, 36));
	if (cv::countNonZero(rectImg) > 1500) {//认为是黄车牌，二值化后字符为黑色

		cv::threshold(plateImgThres, plateImgThres, 128, 255, CV_THRESH_BINARY_INV);
	}
	outputThresImg = plateImgThres;
	//cPlate->plateBinaryImg = plateImgThres;
	//去除左右边框并提取字符
	cv::Mat plateThresh;
	if (plateImg.channels() == 3) {
		cv::cvtColor(plateImg, plateThresh, cv::COLOR_BGR2GRAY);
	}


	bool firstCol = 0;
	int colCount[136];
	std::memset(colCount, 0, sizeof(colCount[0]) * 136);
	for (int i = 0; i < plateThresh.cols; i++) {
		for (int j = 0; j < plateThresh.rows; j++) {
			if (plateThresh.at<uchar>(j, i) > 128) {
				colCount[i]++;
			}
		}
	}
	//(*cPlate).plateBinaryImg = plateThres.clone();
	int average = 0;
	for (int i = 0; i < 136; i++)if (colCount[i] > 3)average++;
	average = average / 7;//字符平均宽度

	uchar startCol = 0;
	uchar endCol = startCol;
	std::vector < std::pair<uchar, uchar>> segmentPos;
	std::pair<uchar, uchar> tempSegm;
	bool inblock = false;

	for (int i = 0; i < 136; i++) {
		if (inblock&&colCount[i] > 3) { //字符块继续遍历
			inblock = true;
			endCol++;
		}
		if (!inblock&&colCount[i] > 3) {//新的字符块开始遍历
			endCol = startCol = i;
			inblock = true;
		}
		if (inblock&&colCount[i] < 3) {//字符块结束遍历
			endCol = i;
			inblock = false;
			//判断字符块是否合格，有以下情况：粘连，不连接的字符，边框，根据是否第一个字符或最后一个字符判断
			if (segmentPos.size() == 0) {//第一个字符
				if ((endCol - startCol) > (average + 4)) {
					startCol += 4;
					endCol += 2;
				}
				if ((endCol - startCol) < (average - 6)) {
					inblock = true;
					continue;
				}
				if ((endCol - startCol) < (average - 4)) {
					endCol += 4;
				}
			}
			else if (segmentPos.size() == 6) {//最后一个字符

			}
			else if (segmentPos.size() >= 7) {//多余的字符
				if ((endCol - startCol) < average);
			}
			else
			{
				if ((endCol - startCol) < (average - 7)) {
					inblock = true;
					continue;
				}
				if ((endCol - startCol) < (average - 6)) {
					startCol -= 3;
					endCol += 3;

				}
			}

			tempSegm = std::make_pair(startCol, endCol);
			segmentPos.push_back(tempSegm);
			cv::Rect2f charsRect;
			cv::RotatedRect charsRRect= cv::RotatedRect(cv::Point(tempSegm.first, 0), cv::Point(tempSegm.second, 0), cv::Point(tempSegm.second, 36));
			util::calcSafeRect(charsRRect, plateThresh.size(), charsRect);
			cv::Mat charImg = cv::Mat(plateThresh,charsRect);
			startCol = i;
			cv::imshow("charImg", charImg);
			//cv::waitKey(0);
		}
		if (!inblock&&colCount[i] < 6) {//空白区域，不做处理

			//inblock = false;
		}
	}
	std::pair<uchar, uchar> tempPos;
	
	for (int i = 0; i < segmentPos.size(); i++) {
		tempPos = segmentPos[i];
		rr.push_back(cv::RotatedRect(cv::Point(tempPos.first, 0), cv::Point(tempPos.second, 0), cv::Point(tempPos.second, 36)));
	}
	
}


void CharsSegment::spatialOstu(cv::Mat &inputOutputImg, float persent, int grid_x, int grid_y) {
	cv::Mat &src = inputOutputImg;
	double globalSigma;
	if (src.channels() == 3) {
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
	}
	double globalThreshVal = util::getThreshVal_Otsu_8u(src, globalSigma);
	int width = src.cols / grid_x;
	int height = src.rows / grid_y;
	std::vector<int> local_globalThreshVal;
	std::vector<double> local_globalSigma;

	for (int i = 0; i < grid_x; i++) {//在水平方向上分grid_x个栏，设为该栏垂直方向上的全局阈值//
									 //效果不佳，噪声极大，检测车牌数锐减
		cv::Mat src_cell = cv::Mat(src, cv::Range::all(), cv::Range(i * width, (i + 1) * width));
		double tempSigma;
		double ThreshVal = util::getThreshVal_Otsu_8u(src_cell, tempSigma);
		local_globalSigma.push_back(tempSigma);
		local_globalThreshVal.push_back(ThreshVal);
	}

	// iterate through grid
	for (int j = 0; j < grid_y; j++) {
		for (int i = 0; i < grid_x; i++) {
			double sigma;
			cv::Mat src_cell = cv::Mat(src, cv::Range(j * height, (j + 1) * height), cv::Range(i * width, (i + 1) * width));
			cv::imshow("src_cell", src_cell);
			double cellThreshVal = util::getThreshVal_Otsu_8u(src_cell, sigma);

			cv::threshold(src_cell, src_cell, cellThreshVal*1.5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);


			//cv::imshow("after src_cell", src_cell);
			//cv::waitKey(0);
		}
	}
}
/**
 * @brief 二值化的通用接口
 * detail description
 * @param inputImg    输入的原图像
 * @param outputImg    输出的二值化图像
 * @return 无
 *     -<em>false</em> fail
 *     -<em>true</em> succeed
 */
void CharsSegment::thresInterface(cv::Mat &inputImg, cv::Mat &outputImg) {
	outputImg = inputImg.clone(); 
	spatialOstu(outputImg, 0, 8, 1);
	//util::Wallner(affinedThresh1, affinedThresh);//基于积分图的快速局部平均阈值算法，S取一定值，边缘较细，效果比局部ostu好，采用
	//cv::adaptiveThreshold(adatativetempImg, adatativetempImg, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, -5);//常数C难以适应全部，淘汰
	//cv::threshold(affinedGray, affinedGray, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);//对光照不均不友好，淘汰
	cv::imshow("spatialOSTU", outputImg);
}


CharsSegment::CharsSegment()
{
}


CharsSegment::~CharsSegment()
{
}
