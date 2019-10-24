#include"stdafx.h"
#pragma once
#include "PlateLocate.h"
#include "Controler.h"
#include<util.h>
#include"CNNRecognizer.h"
#include"CPlate.h"
#include"CharsSegment.h"

#define DEAFAULT_TEMPIMG "D:\\VS\\CPR\\TEMPIMG\\"
PlateLocate::PlateLocate()
{

}

PlateLocate::~PlateLocate()
{
}

std::string PlateLocate::tempPath = "D:/VS/CPR/recoPlate/";//识别车牌的保持路径

/**
 * @brief 车牌定位
 * 车牌定位的入口
 * @param srcImg      原图
 * @param currCPlate     当前车牌信息
 * @return 是否定位成功
 *     -<em>false</em> 定位成功
 *     -<em>true</em> 定位失败
 */
bool PlateLocate::locate(cv::Mat srcImg,std::vector<CPlate> &currCPlates) {
	currSize = srcImg.size();
	
	cv::Mat tempImg;
	preProcess(srcImg, tempImg);
	//Controler::getControler()->showImg(srcImg);
	sobelResearch3(srcImg,currCPlates);

	if (currCPlates.empty()) {
		//cv::imwrite(path + "false.jpg", srcImg);
		return false;//如果识别的车牌数为空，返回false
	}

		
	//显示效果
	#ifdef DEBUG
	if (0) {
		Controler::getControler()->showImg(srcImg);
		}
	#endif // DEBUG
	return true;
}


/**
 * @brief 倾斜校正
 * detail description
 * @param parameter name    parameter description
 * @param parameter name    parameter description
 *
 * @return parameter description
 *     -<em>false</em> fail
 *     -<em>true</em> succeed
 */
void PlateLocate::deskew() {
	//if (currPlates.plateImgs.empty())return;
	//for (auto img : currPlates.plateImgs) {
		
	//}

}

void PlateLocate::sobelOpert(const cv::Mat &srcImg, cv::Mat &outputImg, cv::Size blurSize) {
		

	/*
	cv::Mat blurImg;
	//cv::GaussianBlur(srcImg, blurImg, blurSize, 0);

	cv::Mat sizeImg;
	cv::resize(srcImg, sizeImg, cv::Size(390,118));
	//cv::cvtColor(sizeImg, sizeImg, CV_BGR2GRAY);

	cv::Mat sobelImg;
	//cv::Sobel(sizeImg, sobelImg, CV_8U, 1, 0, 3);
	//cv::Canny(sizeImg, sobelImg, 50, 80);
	
	cv::Mat thresImg;
	//cv::adaptiveThreshold(sizeImg, thresImg, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 9, 0);
	//spatialOstu(sizeImg, 8, 6);
	//cv::threshold(sobelImg, thresImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	util::AdaptiveThereshold(sizeImg, thresImg,-14);
	cv::imshow("-15", thresImg);

	SHOW_IMAGE(thresImg, 1);
	std::vector<std::vector<cv::Point>> contours; 
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(thresImg, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	for (auto contour : contours) {
		cv::RotatedRect rRect=cv::minAreaRect(contour);
		cv::Point2f* vertices = new cv::Point2f[4];
		rRect.points(vertices);
		std::vector<cv::Point>rectcontour;
		for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
		std::vector<std::vector<cv::Point>> rectcontous;
		rectcontous.push_back(rectcontour);
		cv::drawContours(thresImg, rectcontous, 0, cv::Scalar(255, 255, 0), 1);
	}
	

	//cv::adaptiveThreshold(sobelImg, thresImg, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 5, 20);
	//SHOW_IMAGE(thresImg, 1);*/
}

void  PlateLocate::clearUpDownBorder(const cv::Mat &binaryPlateImg, cv::Mat &outputPlateImg, int threshold) {
	outputPlateImg = binaryPlateImg.clone();
	for (size_t i = 0; i < outputPlateImg.rows;i++) {
		uchar* inData = outputPlateImg.ptr<uchar>(i);
		int jumpCount=0;
		for (size_t j = 0; j < outputPlateImg.cols-1; j++)
		{
			if (inData[j] != inData[j + 1])jumpCount++;
		}
		if (jumpCount < threshold)
		{
			for (size_t j = 0; j < outputPlateImg.cols; j++)inData[j] = 0;
		}
	}
}

void  PlateLocate::fineMapping(std::vector<CPlate> &vecCPlate) {
	std::vector<CPlate>::iterator itBegin = vecCPlate.begin();
	for (; itBegin != vecCPlate.end(); ) {
		if (!fineMappingVertical((*itBegin).plateImg, (*itBegin).plateImg,(*itBegin).transformMat,(*itBegin).srcImg)) {
			itBegin = vecCPlate.erase(itBegin);
			//itBegin++;
		}
		else
		{
			fineMappingHorizontal((*itBegin).plateImg, (*itBegin).plateImg,(*itBegin).transformMat, (*itBegin).srcImg);
			itBegin++;
		};
	}
}
double PlateLocate::findThetaUsingRadon(const cv::Mat &plateImgThres) {
	

	//寻找字符矩形
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(plateImgThres, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator itBegin = contours.begin();
	std::vector<cv::Rect> vecRect;
	for (; itBegin != contours.end(); itBegin++) {
		cv::Rect2f rect;
		//	util::calcSafeRect(cv::minAreaRect(*itBegin), plateImgThres.size(), rect);
		rect = cv::boundingRect(*itBegin);
		if (vertifyCharacterSizes(rect, 0.1, 1.2, 100, 1000)) {
			vecRect.push_back(rect);
		}
	}

	std::vector<double> vecTheta;
	if (vecRect.size() > 4) {
		for (int i = 0; i < vecRect.size(); i++) {

			cv::Mat img(plateImgThres, vecRect[i]);
			cv::Mat rectImg = img.clone();
			cv::imshow("rectImg", rectImg);
		//		cv::waitKey(0);


			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(rectImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
			cv::Rect maxRect;
			int max = -1;
			maxRect.height = 0;
			maxRect.width = 0;

			if (contours.size() > 1) {
				for (int j = 0; j < contours.size(); j++) {
					cv::Rect rect = cv::boundingRect(contours[j]);
					if (rect.area() > maxRect.area()) {

						maxRect = rect;

						if (max != -1) {

							cv::drawContours(rectImg, contours, max, cv::Scalar(0), -1, 8);
						}
						max = j;
					}
					else
					{
						cv::drawContours(rectImg, contours, j, cv::Scalar(0), -1);

					}
				}

			}

			int maxLength = 2*rectImg.rows + rectImg.cols+1;
			int num[81] = { 0 };
			for (int theta = -40; theta < 41; theta += 1) {
				double thetaPI = theta / 180.0*CV_PI;
				int point = 0;
				cv::Mat xProjection(1, maxLength, CV_32FC1);
				for (int j = 0; j < rectImg.cols; j++) {
					for (int k = 0; k < rectImg.rows; k++) {
						if (rectImg.at<uchar>(k, j) > 128) {
							float x = std::cos(thetaPI)*j + std::sin(thetaPI)*k + rectImg.rows ;
							float decimal = x - int(x);
							xProjection.at<float>(0, int(x)) = 1;
							if (decimal > 0.5) {
								xProjection.at<float>(0, int(x) + 1) = 1;
							}
						}
					}
				}

				for (int l = 0; l < maxLength; l++) {
					if (xProjection.at<float>(0, l) > 0)point++;
				}
				num[theta + 40] = point;
			}
			int minNum = *(std::min_element(num, num + 81));
			//	int thetafinal=std::distance(num, std::min_element(num, num + 41));
			int thetafinal = 0;
			uchar maxLength2 = 0;
			uchar leftPos = 0;
			uchar rightPos = 0;
			uchar in = 0;
			for (int i = 0; i < 81; i++) {
				if (num[i] > (minNum + 1)) {
					if (in == 1) {
						in = 0;
						uchar tempMaxLenth = rightPos - leftPos;
						if (tempMaxLenth > maxLength2) {
							maxLength2 = tempMaxLenth;
							thetafinal = (rightPos + leftPos) / 2;
						}
					}
					else
					{
						continue;
					}


				}
				else
				{
					if (in == 0) {
						in = 1;
						leftPos = i;
						rightPos = i;
					}
					else
					{
						rightPos = i;
					}

				}

			}
			thetafinal -= 40;
			vecTheta.push_back(thetafinal);
		}


		//剔除可能异常值
		double average = 0.0;
		for (int i = 0; i < vecTheta.size(); i++) {
			average += vecTheta[i];
		}
		average = average / vecTheta.size();
		double stdDeviation=0.0;//标准差
		for (int i = 0; i < vecTheta.size(); i++) {
			stdDeviation += (vecTheta[i] - average)*(vecTheta[i] - average);
		}
		stdDeviation = std::sqrt(stdDeviation / vecTheta.size());

		//累加在偏差范围内的值
		double averageTheta = 0.0;
		uchar size = 0;
		for (int i = 0; i < vecTheta.size(); i++) {
			if ((vecTheta[i] - average) < stdDeviation)
			{
				averageTheta += vecTheta[i];
				size++;
			}
		}
		averageTheta = averageTheta / size;
		return averageTheta;
}
else
	{
		return 0.0;
	}
		
}
bool PlateLocate::fineMappingHorizontal(const cv::Mat &plateImg, cv::Mat &outputThresImg, cv::Mat &transMat, cv::Mat emtireImg) {
	cv::Mat plateImgThres = plateImg.clone();


	cv::imshow("Horizonatal GRAY", plateImgThres);
	cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "-" + "src" + "-" + std::to_string(std::rand()) + ".jpg", plateImgThres);
//	cv::waitKey(0);

	if (plateImgThres.channels() == 3) {
		cv::cvtColor(plateImgThres, plateImgThres, cv::COLOR_BGR2GRAY);
	}
//	util::Wallner(plateImgThres, plateImgThres);


	/*
	cv::Mat cannyImg;
	cv::Canny(plateImgThres, cannyImg, 40, 100);

	int leftPos=-1;
	int  rightPos=-1;
	size_t l = cannyImg.cols-1;
	for (size_t i=0; i < l; i++,l--) {
		uchar leftWhiteCount=0;
		uchar rightWhiteCount = 0;
		for (size_t j=0; j < cannyImg.rows&&leftPos==-1; j++) {
			if (cannyImg.at<uchar>(j, i) == 255)leftWhiteCount++;
			if (cannyImg.at<uchar>(j, i+1) == 255)leftWhiteCount++;
		}

		for (size_t j=0; j < cannyImg.rows&&rightPos==-1; j++) {
			
			if (cannyImg.at<uchar>(j, l - 1) == 255)rightWhiteCount++;
			if (cannyImg.at<uchar>(j,l) == 255)rightWhiteCount++;

		}

		if (leftWhiteCount > 10)leftPos = i;
		if (rightWhiteCount > 10)rightPos =l;
		if (leftPos != -1 && rightPos != -1)break;
	}
	if (leftPos == -1)leftPos = 0;
	if (rightPos == -1)rightPos = 136;
	*/

	

	util::spatialOstu(plateImgThres, 2, 2);
	cv::imshow("canny Img", plateImgThres);
//	cv::waitKey(0);




	


	//判断是否黄车牌，翻转灰度
	cv::Mat rectImg = cv::Mat(plateImgThres, cv::Rect(24, 0, 88, 28));
	if (cv::countNonZero(rectImg) > 1300) {//认为是黄车牌，二值化后字符为黑色

		cv::threshold(plateImgThres, plateImgThres, 128, 255, CV_THRESH_BINARY_INV);

	}
	util::clearUpDownBorder(plateImgThres, plateImgThres, 12);

	//	outputThresImg = plateImgThres;


	

	//用二值图的差值查找左右边界。
	int leftPos = -1;
	int  rightPos = -1;
	size_t l = plateImgThres.cols - 1;
	for (size_t i = 0; i < l; i++, l--) {
		uchar leftWhiteCount = 0;
		uchar rightWhiteCount = 0;
		for (size_t j = 0; j < plateImgThres.rows&&leftPos == -1; j++) {
			if ((plateImgThres.at<uchar>(j, i) - plateImgThres.at<uchar>(j, i + 1)) < 0)leftWhiteCount++;
			if ((plateImgThres.at<uchar>(j, i + 1) - plateImgThres.at<uchar>(j, i + 2)) < 0)leftWhiteCount++;
		}

		for (size_t j = 0; j < plateImgThres.rows&&rightPos == -1; j++) {

			if ((plateImgThres.at<uchar>(j, l) - plateImgThres.at<uchar>(j, l - 1)) < 0)rightWhiteCount++;
			if ((plateImgThres.at<uchar>(j, l - 1) - plateImgThres.at<uchar>(j, l - 2)) < 0)rightWhiteCount++;

		}

		if (leftWhiteCount > 5)leftPos = i;
		if (rightWhiteCount >5)rightPos = l;
		if (leftPos != -1 && rightPos != -1)break;
	}
	if (leftPos == -1 || leftPos < 4) {
		leftPos = 0;
	}
	else
	{
		leftPos = leftPos - 4;
	}
	if (rightPos == -1 || rightPos > 132) {
		rightPos = 136;
	}
	else
	{
		rightPos = rightPos + 4;
	}




	cv::imshow("plateImgThres", plateImgThres);
	cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "-" + "binary" + "-" + std::to_string(std::rand()) + ".jpg", plateImgThres);
//	cv::waitKey(0);		


	double theta=findThetaUsingRadon(plateImgThres);
	
	double k = std::tan((theta + 90) / 180.0 * CV_PI);

	//要注意opencv中的坐标系
	std::vector<cv::Point2f> srcPoint;
	srcPoint.push_back(cv::Point2f(leftPos, -1));	
	srcPoint.push_back(cv::Point2f(leftPos + 36.0 / k, 36 / std::cos(std::abs((theta) / 180.0 * CV_PI))+1));		
	srcPoint.push_back(cv::Point2f(rightPos, -1));
	std::vector<cv::Point2f> dstPoint;
	dstPoint.push_back(cv::Point2f(0, 0));
	dstPoint.push_back(cv::Point2f(0, 36.0 ));
	dstPoint.push_back(cv::Point2f(136, 0));

	cv::Mat affineTransform = cv::getAffineTransform(srcPoint, dstPoint);
	cv::Mat num = (cv::Mat_<double>(1, 3) << 0, 0, 1);
	affineTransform.push_back(num);

	transMat = affineTransform * transMat;
	cv::Mat transformImg;
	cv::Mat transform2 = transMat(cv::Range(0, 2), cv::Range(0, 3));

	cv::Mat resultImg;
	cv::warpAffine(emtireImg, resultImg, transform2, cv::Size(136, 36),cv::INTER_LINEAR,cv::BORDER_REPLICATE);
//	util::ACE(resultImg);
	outputThresImg = resultImg;
	cv::imshow("plateImgThres2", resultImg);
//	cv::waitKey(0);
		
	return true;
}

bool PlateLocate::fineMappingVertical(const cv::Mat &srcImg, cv::Mat &outputImg, cv::Mat &transMat, cv::Mat emtireImg) {
	cv::Mat sizeImg=srcImg.clone();
//	cv::resize(srcImg, sizeImg, cv::Size(136, 36));

	cv::cvtColor(sizeImg, sizeImg, cv::COLOR_BGR2GRAY);



	cv::imshow("srcImg", sizeImg);
//cv::waitKey(0);
	cv::Mat thresImg;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Rect> rects;
	cv::Rect rect;
	std::vector<cv::Point> upPoints;
	std::vector<cv::Point> lowPoints;
	char upper = 10;
	char lowper = -12;
	char step = 4;
	std::vector<cv::Rect> vecRect;
	for (; lowper < upper; lowper += step) {
		util::AdaptiveThereshold(sizeImg, thresImg, lowper);
		util::clearUpDownBorder(thresImg, thresImg, 7);
		cv::findContours(thresImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		cv::Mat contoursImg = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);
		cv::drawContours(contoursImg, contours, -1, cv::Scalar(255), 1);
		cv::resize(contoursImg, contoursImg, cv::Size(408, 108));
		cv::imshow("contours", contoursImg);
		cv::Mat debugImg;
		cv::resize(thresImg, debugImg, cv::Size(408, 108));
		cv::imshow("debugImg", debugImg);
		for (auto contour : contours) {
			rect = cv::boundingRect(contour);
			if (vertifyCharacterSizes(rect,0.2,1.2,100,1400)) {
				vecRect.push_back(rect);

				cv::Mat debugImg2 = sizeImg.clone();
				cv::rectangle(debugImg2, rect, cv::Scalar(255, 255, 255), 2);
				cv::imshow("rectangle", debugImg2);
	//			cv::waitKey(0);
			}
			else //仅作过程展示用
			{
				cv::Mat badImg = sizeImg.clone();
				cv::rectangle(badImg, rect, cv::Scalar(255, 255, 255), 2);
				cv::resize(badImg, badImg, cv::Size(408, 108));
				cv::imshow("badImg", badImg);
	//			cv::waitKey(0);
			}
		}
	}

	

	if (vecRect.size()<8) {
		vecRect.clear();
		lowper = -14;
		upper = 10;
		for (; lowper < upper; lowper += step) {
			util::AdaptiveThereshold(sizeImg, thresImg, lowper);
			cv::threshold(thresImg, thresImg, 126, 255, CV_THRESH_BINARY_INV);
			util::clearUpDownBorder(thresImg, thresImg,7);
			cv::findContours(thresImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			cv::Mat contoursImg = cv::Mat::zeros(srcImg.rows, srcImg.cols, CV_8UC1);
			cv::drawContours(contoursImg, contours, -1, cv::Scalar(255), 1);
			cv::resize(contoursImg, contoursImg, cv::Size(408, 108));
			cv::imshow("contours", contoursImg);
			cv::Mat debugImg;
			cv::resize(thresImg, debugImg, cv::Size(408, 108));
			cv::imshow("debugImg", debugImg);
			for (auto contour : contours) {
				rect = cv::boundingRect(contour);
				if (vertifyCharacterSizes(rect, 0.2, 1.2, 100, 1400)) {
					vecRect.push_back(rect);

					cv::Mat debugImg2 = sizeImg.clone();
					cv::rectangle(debugImg2, rect, cv::Scalar(255, 255, 255), 2);
					cv::imshow("rectangle", debugImg2);
		//			cv::waitKey(0);
				}
				else
				{
					cv::Mat badImg = sizeImg.clone();
					cv::rectangle(badImg, rect, cv::Scalar(255, 255, 255), 2);
					cv::resize(badImg, badImg, cv::Size(408, 108));
					cv::imshow("badImg", badImg);
		//			cv::waitKey(0);
				}
			}
		}
	}
	//进一步筛选矩形，并取点	
	if (vecRect.size() > 8) {
		
		//找到矩形最集中的高度
		uchar size = vecRect.size();
		uchar height[13] = { 0 };
		for (int i = 0; i < size; i++) {
			height[vecRect[i].height/3]++;
		}
		uchar maxPos=0;
		uchar maxValue=0;
		for (int i = 0; i < 13; i++) {
			if (height[i] > maxValue) {
				maxValue = height[i];
				maxPos = i;
			}
		}
		std::vector<cv::Rect>::iterator rectBegin = vecRect.begin();
		maxPos *= 3;
		for (; rectBegin != vecRect.end(); ) {
			if (((*rectBegin).height - maxPos) > 3|| ((*rectBegin).height - maxPos) <-1) {
				rectBegin = vecRect.erase(rectBegin); 
			}
			else
			{
				//去除左右两边缘的矩形
				if ((*rectBegin).x < srcImg.cols / 6) {
					rectBegin = vecRect.erase(rectBegin);
					continue;
				}
				if ((*rectBegin).x > srcImg.cols * 5  / 6) {
					rectBegin = vecRect.erase(rectBegin);
					continue;
				}

				cv::Mat badImg = sizeImg.clone();
				cv::rectangle(badImg, *rectBegin, cv::Scalar(255, 255, 255), 2);
				cv::imshow("candidate", badImg);
		//		cv::waitKey(0);

				upPoints.push_back((*rectBegin).tl());
				lowPoints.push_back((*rectBegin).br());
				rectBegin++;
			}
		}		

				
	}
	else
	{
		cv::imshow("false mapping rect num", sizeImg);
	//	cv::waitKey(0);
		return false;
	}
	

	if (upPoints.size() < 3 || lowPoints.size() < 3) { 
		cv::imshow("false mapping points num", sizeImg);
	//	cv::waitKey(0);
		return false;
	}

	cv::Vec4f line;
	float k1 = 0, k2 = 0;
	cv::fitLine(upPoints, line, cv::DIST_HUBER, 0, 0.01, 0.01);
	float topVx = line[0];
	float topVy = line[1];
	float topX = line[2];
	float topY = line[3];
	k1 = line[1] / line[0];

	


	cv::fitLine(lowPoints, line, cv::DIST_HUBER, 0, 0.01, 0.01);
	float lowVx = line[0];
	float lowVy = line[1];
	float lowX = line[2];
	float lowY = line[3];
	k2 = line[1] / line[0];

	if (k1 > 1 || k2 > 1 || k1 < -1 || k2 < -1) {
		cv::imshow("false mapping k wrong", sizeImg);
		//		cv::waitKey(0);
		return false;
	}
	float k = (k1 + k2) / 2;

	//实际上求直线x=0与y=kx+c相交时的y1值，以（0，y1）作为边线左端的坐标
	int topLeftY = static_cast<int>((-topX * k) + topY);//需要验证安全范围
	//实际上是求当y轴平移136个像素时，直线x=136与y轴相交的y2值，以（136，y2）作为边线右端的坐标
	int topRightY = static_cast<int>(((136 - topX) *k) + topY);
//	cv::line(sizeImg, cv::Point(0, topLeftY), cv::Point(136, topRightY), cv::Scalar(255, 255, 255), 2);

	//实际上求直线x=0与y=kx+c相交时的y1值，以（0，y1）作为边线左端的坐标
	int lowLeftY = static_cast<int>((-lowX * k) + lowY);
	//实际上是求当x轴平移136个像素时，直线x=136与y轴相交的y2值，以（136，y2）作为边线右端的坐标
	int lowRightY = static_cast<int>(((136 - lowX) * k) + lowY);
//	cv::line(sizeImg, cv::Point(0, lowLeftY), cv::Point(136, lowRightY), cv::Scalar(255, 255, 255), 2);

	std::vector<cv::Point2f> srcPoints;
	std::vector<cv::Point2f> dstPoints;
	srcPoints.push_back(cv::Point2f(0, topLeftY-1));
	srcPoints.push_back(cv::Point2f(0, lowLeftY+1));
	srcPoints.push_back(cv::Point2f(136, lowRightY+1));

	dstPoints.push_back(cv::Point2f(0,0));
	dstPoints.push_back(cv::Point2f(0, 36));
	dstPoints.push_back(cv::Point2f(136, 36));

	cv::Mat transform = cv::getAffineTransform(srcPoints, dstPoints);
	//double num[3] = { 0,0,1 };
	cv::Mat num = (cv::Mat_<double>(1, 3) << 0,0, 1);
	transform.push_back(num);
//	num = 0;
//	transform.push_back(num);
//	num = 1;
//	transform.push_back(num);
	transMat = transform*transMat;
	cv::Mat transformImg;
	cv::Mat transform2 = transMat(cv::Range(0, 2), cv::Range(0, 3));
 	cv::warpAffine(emtireImg, transformImg, transform2, cv::Size(136, 36), cv::INTER_LINEAR);
	outputImg = transformImg.clone();
	
	cv::imshow("ddd", sizeImg);
	cv::imshow("transformImg", outputImg);
 // 	cv::waitKey(0);
	return true;
}

// this spatial_ostu algorithm are robust to 
// the plate which has the same light shine, which is that
// the light in the left of the plate is strong than the right.
  /**
  * @brief 在图像局部使用ostu二值化
  * detail description
  * @param  _src    输入的原图
  * @param  grid_x    水平方向栅格数目
  * @param grid_y  垂直方向栅格数目
  */
void PlateLocate::spatialOstu(cv::Mat &inputOutputImg, float persent, int grid_x, int grid_y) {
	cv::Mat src = inputOutputImg;
	double globalSigma;
	double globalThreshVal = util::getThreshVal_Otsu_8u(inputOutputImg,globalSigma);
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
			cv::imshow("src_cell",src_cell);
			double cellThreshVal=util::getThreshVal_Otsu_8u(src_cell, sigma);
			if (cellThreshVal>globalThreshVal*(persent+0.6) ){//再加上全局阈值的判断
				cv::threshold(src_cell, src_cell, 77, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
				//cv::threshold(src_cell, src_cell, 255, 255,  CV_THRESH_BINARY);
				//cv::adaptiveThreshold(src_cell, src_cell, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, -15);
			}
			else
			{
				cv::threshold(src_cell, src_cell, 77, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
				//cv::adaptiveThreshold(src_cell, src_cell, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, -15);
			}
			
			cv::imshow("after src_cell", src_cell);
//			cv::waitKey(0);
		}
	}
}


//判断矩形是否是字符区域的矩形
bool PlateLocate::vertifyCharacterSizes(const cv::Rect &Rect,float minRatio,float maxRatio,int minArea,int maxArea) {
	float ratio = Rect.width / (static_cast<float>(Rect.height));
	float area = Rect.area();
	if ((ratio > minRatio && ratio < maxRatio) && (area > minArea && area < maxArea)) {//考虑到倾斜情况，最大面积需增大
		return true;
	}
	else
	{
		return false;
	}

}

bool PlateLocate::vertifySizesTemp(const cv::RotatedRect &rRect) {
	float ratioMin = 0.8;//计算得到最小比例
	float ratioMax = 5.9;//计算得到最大比例

	int areaMin = 120;
	int areaMax = 30000;

	//计算得到候选矩形长宽比和面积
	float ratio = float(rRect.size.width) / float(rRect.size.height);
	int area = rRect.size.width*rRect.size.height;
	if (ratio < 1) {
		ratio = 1 / ratio;
		if (ratio < ratioMin || ratio > ratioMax || area<areaMin || area > areaMax
			|| rRect.angle > -60)
		{//尽量用or避免用and//||(rRect.angle<-30&&rRect.angle>-70)
			return false;
		}
	}
	else if (ratio < ratioMin || ratio > ratioMax || area<areaMin || area > areaMax
		|| rRect.angle < -40) {//尽量用or避免用and//||(rRect.angle<-30&&rRect.angle>-70)
		return false;
	}

	return true;
}




/**
 * @brief K均值聚类分割车牌扩大区域
 * 现阶段效果不好，暂时不用，后期可考虑使用高斯模糊和ACE算法增强图像，或者其他分割方法。
 * @param srcImg    扩大化的车牌图像
 * @param blurSize    高斯模糊核大小
 * @param clusterNum    聚类的类别总数
 */
void PlateLocate::kMean(cv::Mat srcImg,cv::Size blurSize, int clusterNum) {
	//五个颜色，聚类之后的颜色随机从这里面选择
	cv::Scalar colorTab[] = {
			cv::Scalar(0,0,255),
			cv::Scalar(0,255,0),
			cv::Scalar(255,0,0),
			cv::Scalar(0,255,255),
			cv::Scalar(255,0,255)
	};
	if (!srcImg.data)return ;
	cv::Mat srcImage2 = srcImg.clone();
	cv::GaussianBlur(srcImage2, srcImage2, blurSize, 0);
	cv::cvtColor(srcImage2, srcImage2, CV_BGR2HSV);
	std::vector<cv::Mat> planes;
	cv::split(srcImage2, planes);
	cv::Mat srcImage = planes[2].clone();
	SHOW_IMAGE(srcImage, 1);
	int width = srcImage.cols;//图像的宽
	int height = srcImage.rows;//图像的高
	int channels = srcImage.channels();//图像的通道数
	//初始化一些定义
	int sampleCount = width * height;//所有的像素
	int clusterCount = 3;//分类数
	cv::Mat points(sampleCount, channels, CV_32F, cv::Scalar(10));//points用来保存所有的数据
	cv::Mat labels;//聚类后的标签
	cv::Mat center(clusterNum, 1, points.type());//聚类后的类别的中心
	//将图像的RGB像素转到到样本数据
	int index;
	for (int i = 0; i < srcImage.rows; i++)
	{
		for (int j = 0; j < srcImage.cols; j++)
		{
			index = i * width + j;
			uchar bgr = srcImage.at<uchar>(i, j);
			//将图像中的每个通道的数据分别赋值给points的值
			points.at<float>(index, 0) = static_cast<int>(bgr);
			//	points.at<float>(index, 1) = static_cast<int>(bgr[1]);
			//	points.at<float>(index, 2) = static_cast<int>(bgr[2]);
		}
	}
	//运行K-means算法
	//MAX_ITER也可以称为COUNT最大迭代次数，EPS最高精度,10表示最大的迭代次数，0.1表示结果的精确度
	cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 0.01);
	kmeans(points, clusterNum, labels, criteria, 3, cv::KMEANS_PP_CENTERS, center);
	//显示图像分割结果
	cv::Mat result = cv::Mat::zeros(srcImage.size(), srcImg.type());//创建一张结果图
	for (int i = 0; i < srcImage.rows; i++)
	{
		for (int j = 0; j < srcImage.cols; j++)
		{
			index = i * width + j;
			int label = labels.at<int>(index);//每一个像素属于哪个标签
			result.at<cv::Vec3b>(i, j)[0] = colorTab[label][0];//对结果图中的每一个通道进行赋值
			result.at<cv::Vec3b>(i, j)[1] = colorTab[label][1];
			result.at<cv::Vec3b>(i, j)[2] = colorTab[label][2];
		}
	}
	imshow("Kmeans", result);
	cv::waitKey(0);
}
void PlateLocate::preProcess(const cv::Mat &preImg, cv::Mat &outImg) {
	//图像预处理
	int newrows = preImg.rows*IDEA_WIDTH / preImg.cols;
	cv::resize(preImg, outImg, cv::Size(IDEA_WIDTH, newrows));	

	/*//自定义灰度化
	std::vector<cv::Mat> vecMat;
	cv::split(processedImg, vecMat);
	for (int i = 0; i < processedImg.rows; i++) {
		for ()
	}
	*/
	//cv::GaussianBlur(outImg, outImg, cv::Size(DEAFAULT_GAUSIIBLUR_SIZE, DEAFAULT_GAUSIIBLUR_SIZE), 0, 0, cv::BORDER_DEFAULT);	
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
void PlateLocate::plateJudge(const cv::Mat &srcImg,std::vector<cv::RotatedRect> &rotatedRects) {
	
//	SVMJudge svmJudge;
	cv::Mat plateImg;
	cv::Mat saveImg;
	//char count;
	for (int i = 0; i < rotatedRects.size(); i++) {
		
		cv::Mat transMat;
		util::getRotatedRectArea(srcImg, rotatedRects[i], plateImg, transMat,false);
		float score;
		if (svm.startJudge(plateImg,score)) {
			//currPlates.plateImgs.push_back(plateImg);
			
			//currPlates.rRects.push_back(rotatedRects[i]);
			util::getRotatedRectArea(srcImg, rotatedRects[i], saveImg, transMat);
			//cv::imwrite(tempPath+std::to_string(Controler::getControler()->getCurrImgCount())+ std::to_string(i)+".jpg", saveImg);
			//Controler::getControler()->showImg(plateImg);
			
		}
		
	}
	
}

void PlateLocate::sobelProcess(const cv::Mat&sobelImg, cv::Mat &outputImg) {

	//二值化
	cv::Mat sobelImage = sobelImg.clone();
	cv::Mat preThreshold = sobelImg.clone();
	//spatialOstu(sobelImage, 0.7, 4, 3);
	//cv::threshold(sobelImage, sobelImage, 77, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
	//util::AdaptiveThereshold(sobelImage, sobelImage,-30);//局部白点太多
	//cv::adaptiveThreshold(sobelImage, sobelImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 0);
//	cv::imshow("ddd2", preThreshold);
	cv::Mat tempElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 3));
	cv::morphologyEx(sobelImage, sobelImage, cv::MORPH_ERODE, tempElement);
	
	//删除大轮廓
	std::vector<std::vector<cv::Point>> contours;
	cv::RotatedRect rotatedRect;
	cv::findContours(sobelImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator it = contours.begin();
	int widthFilter = sobelImg.cols / 10;
	int heightFilter = sobelImg.rows / 6;
	int areaFilter = sobelImg.cols*sobelImg.rows / 30;
	for (; it != contours.end(); it++) {
		cv::Rect rect = cv::minAreaRect(*it).boundingRect();
		if (rect.width > widthFilter ||rect.height>heightFilter|| rect.area() > areaFilter) {
			qDebug() << rect.width << "   " << rect.height << "  " << rect.area();
		
			/*
			cv::Mat temp = sobelImage.clone();
			std::vector<std::vector<cv::Point>> vecPoint;
			vecPoint.push_back(*it);
			cv::fillPoly(preThreshold, vecPoint, cv::Scalar(0));
			cv::rectangle(temp,rect,cv::Scalar(255));
			cv::imshow("ddd3", temp);
		//	cv::waitKey(0);*/
			for (auto point : *it) {
				int leftX, x, rightX;
				leftX= x=rightX = point.x;
				int downY, y, topY;
				downY= y= topY= point.y;
				int llX, rrX;
				llX = rrX = point.x;

				int ddY, ttY;
				ddY = ttY = point.y;

				if(x-1>=0) leftX = x - 1;
				if (x+ 1 < preThreshold.cols) rightX = x + 1;
				if (y - 1 >= 0) downY = y - 1;
				if (y + 1 < preThreshold.rows) topY = y + 1;

				if (x - 2 >= 0) llX = x - 2;
				if (x + 2 < preThreshold.cols) rrX = x + 2;
				if (y - 2 >= 0) ddY = y - 2;
				if (y + 2 < preThreshold.rows) ttY = y + 2;


				preThreshold.at<uchar>(topY, leftX) = 0;
				preThreshold.at<uchar>(topY, x) = 0;
				preThreshold.at<uchar>(topY, rightX ) = 0;
				preThreshold.at<uchar>(y, leftX) = 0;
				preThreshold.at<uchar>(y, x) = 0;
				preThreshold.at<uchar>(y, rightX) = 0;
				preThreshold.at<uchar>(downY, leftX) = 0;
				preThreshold.at<uchar>(downY, x) = 0;
				preThreshold.at<uchar>(downY, rightX) = 0;

				preThreshold.at<uchar>(ddY, x) = 0;
				preThreshold.at<uchar>(ttY, x) = 0;

				preThreshold.at<uchar>(y, llX) = 0;
				preThreshold.at<uchar>(y, rrX) = 0;
			}

		}

	}

	outputImg = preThreshold;


	/*
	double sigma;
	int thresValue = util::getThreshVal_Otsu_8u(sobelImg, sigma);
	qDebug() << "otsu value" << thresValue;
	cv::threshold(sobelImage, sobelImage, thresValue, 255, cv::THRESH_BINARY);
	cv::Mat morphImg;


	
	
	//删除小轮廓,并储存可能含有车牌的正置矩形
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(27, 3));
	cv::morphologyEx(sobelImage, morphImg, cv::MORPH_CLOSE, element);
	element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 15));
	cv::morphologyEx(morphImg, morphImg, cv::MORPH_OPEN, element);
	contours.clear();
	cv::findContours(morphImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	it = contours.begin();
	
	for (; it != contours.end(); it++) {
		cv::Rect2f rect;
		util::calcSafeRect(cv::minAreaRect(*it),preThreshold.size(), rect);
		if (rect.width < 41 ||rect.height<13|| rect.area() < 800) {
			std::vector<std::vector<cv::Point>> vecPoint;
			vecPoint.push_back(*it);
			cv::fillPoly(preThreshold, vecPoint, cv::Scalar(0));
			cv::fillPoly(sobelImage, vecPoint, cv::Scalar(0));
		}
		else
		{
			vecRect.push_back(rect);
			cv::Mat tempImg = cv::Mat(preThreshold, rect);
			cv::imshow("rect", tempImg);
			tempImg = cv::Mat(morphImg, rect);
			cv::imshow("temp", tempImg);
			cv::waitKey(0);
		}
	}  
	
	cv::imshow("ddd", sobelImage);
	cv::waitKey(0);
	*/
}

void PlateLocate::sobelResearch3(const cv::Mat &inputImg, std::vector<CPlate>&currCPlates) {
	cv::Mat tempImg;
	tempImg = inputImg;
	cv::Mat blurImg;
	//cv::GaussianBlur(tempImg, blurImg, cv::Size(3, 3), 0);
	

	cv::Mat grayImg;
	cv::Mat preProceImg;//先为预处理图像，后期添加可能矩形轮廓并显示
	cv::Mat sobelImg;
	//preProcess(inputImg, preProceImg);//后期应与外面的preProcess处理过程不同	
	preProceImg = tempImg.clone();
	std::vector<cv::Mat>planes;
	cv::split(preProceImg, planes);
	grayImg = planes[0].clone();

	int nr = preProceImg.rows;
	int nc1 = planes[0].cols;
	if (planes[0].isContinuous()) {
		nc1 = nc1 * nr;
		nr = 1;
	}
	
	for (int i = 0; i < nr; i++) {
		// 每一行图像的指针
		uchar* inData0 = planes[0].ptr<uchar>(i);
		uchar* inData1 = planes[1].ptr<uchar>(i);
		uchar* inData2 = planes[2].ptr<uchar>(i);
		uchar* inData3 = grayImg.ptr<uchar>(i);
		for (int j = 0; j < nc1; j++) {
			inData0[j] *= 0;
			inData1[j] *= 0.6;
			inData2[j] *= 0.6;
			inData3[j] = cv::saturate_cast<uchar>(inData0[j] + inData1[j] + inData2[j]);
			//	if (inData3[j] < 20) { inData3[j] = 0; }
			//	else if (inData3[j] > 180) { inData3[j] = 255; }
			//	else
			//	{
			//		inData3[j] = cv::saturate_cast<uchar>(inData3[j] /180.0 * 255);
			//	}

		}
	}
	
	//cv::merge(planes, preProceImg);

	//cv::cvtColor(preProceImg, grayImg, cv::COLOR_BGR2GRAY);//改动
	//cv::Sobel(grayImg, sobelImg, CV_8U, 1, 0, 3);
	cv::Sobel(grayImg, sobelImg, CV_16S, 1, 0, 3);
	convertScaleAbs(sobelImg, sobelImg);
	cv::imshow("sobelImg", sobelImg);
//	cv::Mat sobelImggray = sobelImg.clone();
//	util::contrastEnhance(sobelImg, sobelImg, 5);
	
	if (sobelImg.isContinuous()) {
		nc1 = nc1 * nr;
		nr = 1;
	}
	for (int i = 0; i < nr; i++) {
		// 每一行图像 的指针
		uchar* inData0 = sobelImg.ptr<uchar>(i);
	//	uchar* inData1 = sobelImggray.ptr<uchar>(i);
		for (int j = 0; j < nc1; j++) {

			if (inData0[j] < 40) { inData0[j] = 0; }
			else if (inData0[j] > 170) { inData0[j] = 255; }
			else
			{
				inData0[j] = cv::saturate_cast<uchar>(inData0[j] / 180.0 * 255);
			}

		}


	}

	//cv::medianBlur(sobelImg, sobelImg, 3);
//	cv::imshow("sobelGray", sobelImg);
//	cv::imshow("sobelGrayExtra", sobelImggray);
	//util::getImgPoint(sobelImg, "imgPoint");

//	spatialOstu(sobelImggray, 0.7, 8, 12);
//	cv::imshow("thressobelImggray", sobelImggray);

	srcImg = tempImg;
	cv::threshold(sobelImg, sobelImg, 255, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);
	//spatialOstu(sobelImg, 1.5, 6, 6);
	sobelProcess(sobelImg, sobelImg);
	for (int i = 2; i <6; i++) {
	//	if (i > 2) {
			cv::resize(sobelImg, blurImg, cv::Size(tempImg.cols * 2 / i, tempImg.rows * 2 / i));
	//	}
	//	else
	//	{
	//		blurImg = sobelImg.clone();
	//	}
		
		sizeI = 2.0/i;
	//	cv::threshold(blurImg, blurImg, 128, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);

		sobelFrtResearch1(preProceImg,grayImg, blurImg, currCPlates);
		//if (rRects.size() <= 0)cv::imwrite("D:/VS/CPR/falseImg/" + std::to_string(Controler::getControler()->getCurrImgCount()) + ".jpg",inputImg);
		
		
	}
//	findEdgeDeskew(currCPlates);
	fineMapping(currCPlates);
}


void PlateLocate::sobelResearch(const cv::Mat &inputImg,  std::vector<CPlate>&currCPlates) {


	cv::Mat grayImg;
	cv::Mat sobelImg;
	cv::Mat processedImg = inputImg.clone();
	cv::cvtColor(processedImg, grayImg, cv::COLOR_BGR2GRAY);//改动
	
	cv::GaussianBlur(grayImg, grayImg, cv::Size(5, 5), 0);
	//cv::Sobel(grayImg, sobelImg, CV_8U, 1, 0);
	cv::Sobel(grayImg, sobelImg, CV_16S, 1, 0,3);
	//cv::Scharr(grayImg, sobelImg, CV_16S, 1, 0);
	convertScaleAbs(sobelImg, sobelImg);
	cv::Mat processedSobel;
	std::vector<cv::Rect2f> rect;
	sobelProcess(sobelImg, processedSobel);

	sobelFrtResearch(processedImg, processedSobel, currCPlates);
	
	//sobelSecResearch(processedImg, sobelImg,  currCPlates, hitArea);//使用sobel算子及其他形态学算子查找图片剩余区域
	//sobelRefineResearch(preProceImg, rRects);
	findEdgeDeskew(currCPlates);

}

void PlateLocate::sobelResearch1(const cv::Mat &inputImg,std::vector<CPlate>&currCPlates) {
	
	cv::Mat grayImg;
	cv::Mat preProceImg;//先为预处理图像，后期添加可能矩形轮廓并显示
	cv::Mat sobelImg;
	//preProcess(inputImg, preProceImg);//后期应与外面的preProcess处理过程不同	
	preProceImg = inputImg.clone();
	std::vector<cv::Mat>planes;
	cv::split(preProceImg, planes);
	grayImg = planes[0].clone();

	int nr = preProceImg.rows;
	int nc1 = planes[0].cols;
	if (planes[0].isContinuous()) {
		nc1 = nc1 * nr;
		nr = 1;
	}
	for (int i = 0; i < nr; i++) {
		// 每一行图像的指针
		uchar* inData0 = planes[0].ptr<uchar>(i);
		uchar* inData1 = planes[1].ptr<uchar>(i);
		uchar* inData2 = planes[2].ptr<uchar>(i);
		uchar* inData3 = grayImg.ptr<uchar>(i);
		for (int j = 0; j < nc1; j++) {
			inData0[j] *= 0;
			inData1[j] *= 0.6;
			inData2[j] *= 0.6;
			inData3[j] = cv::saturate_cast<uchar>(inData0[j] + inData1[j] + inData2[j]);
		//	if (inData3[j] < 20) { inData3[j] = 0; }
		//	else if (inData3[j] > 180) { inData3[j] = 255; }
		//	else
		//	{
		//		inData3[j] = cv::saturate_cast<uchar>(inData3[j] /180.0 * 255);
		//	}
			
		}
	}
	


	//cv::merge(planes, preProceImg);

	
	//cv::cvtColor(preProceImg, grayImg, cv::COLOR_BGR2GRAY);//改动
	//cv::Sobel(grayImg, sobelImg, CV_8U, 1, 0, 3);
	cv::Sobel(grayImg, sobelImg, CV_16S, 1, 0, 3);
	
	//cv::Sobel(grayImg, sobelImg, CV_16S, 1,0, 3);
	
	//cv::Scharr(grayImg, sobelImg, CV_16S, 1, 0);
	convertScaleAbs(sobelImg, sobelImg);
	//cv::equalizeHist(sobelImg, sobelImg);
	cv::imshow("sobelImg", sobelImg);
	cv::Mat sobelImggray = sobelImg.clone();

	if (sobelImg.isContinuous()) {
		nc1 = nc1 * nr;
		nr = 1;
	}
	for (int i = 0; i < nr; i++) {
		// 每一行图像 的指针
		uchar* inData0 = sobelImg.ptr<uchar>(i);
		uchar* inData1 = sobelImggray.ptr<uchar>(i);
		for (int j = 0; j < nc1; j++) {
			
			if (inData0[j] < 20) { inData0[j] = 0; }
			else if (inData0[j] > 100) { inData0[j] = 255; }
			else
			{
				inData0[j] = cv::saturate_cast<uchar>(inData0[j] / 150.0 * 255);
			}

		}



		for (int j = 0; j < nc1; j++) {

			if (inData1[j] < 40) { inData1[j] = 0; }
			else if (inData1[j] > 180) { inData1[j] = 255; }
			else
			{
				inData1[j] = cv::saturate_cast<uchar>(inData1[j] / 180.0 * 255);
			}

		}

	}

	
	//cv::medianBlur(sobelImg, sobelImg, 3);
	cv::imshow("sobelAftergray", sobelImg);
	cv::imshow("sobelImggray", sobelImggray);
	//util::getImgPoint(sobelImg, "imgPoint");
	//cv::waitKey(0);
	spatialOstu(sobelImggray, 0.7, 8, 12);
	cv::imshow("thressobelImggray", sobelImggray);
	
	sobelFrtResearch1(preProceImg,grayImg,sobelImg,currCPlates);
	//if (rRects.size() <= 0)cv::imwrite("D:/VS/CPR/falseImg/" + std::to_string(Controler::getControler()->getCurrImgCount()) + ".jpg",inputImg);
	sobelSecResearch(preProceImg,sobelImg,currCPlates);
//	sobelRefineResearch(preProceImg, rRects);
	findEdgeDeskew(currCPlates);      

}


/**
 * @brief 使用查找边线的方式精定位矩形并进行仿射变换，效果较差
 * detail description
 * @param parameter name    parameter description
 * @param parameter name    parameter description
 *
 * @return parameter description
 *     -<em>false</em> fail
 *     -<em>true</em> succeed
 */
uchar PlateLocate::findEdgeDeskew(std::vector<CPlate>&currCPlates) {
	

	std::vector<CPlate>::iterator iterBegin = currCPlates.begin();
	cv::Mat sobelXImg;
	cv::Mat thresSobelXImg;
	cv::Mat grayImg;
	cv::Mat morphImg;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 3));

	int i = 0;
	//for (; iterBegin != currCPlates.end(); iterBegin++) {
	while(iterBegin!=currCPlates.end()){
		char failure = 0;//后期改为enum赋值
		CPlate *cPlate = &(*iterBegin);
		cv::Mat tempImg = (*cPlate).plateImg.clone();
		
		cv::resize(tempImg, tempImg, cv::Size(136, tempImg.size().height*136/ tempImg.size().width),0,0,cv::INTER_CUBIC);
		cv::cvtColor(tempImg, grayImg, CV_BGR2GRAY);
		cv::Sobel(grayImg, sobelXImg, CV_16S, 1, 0);
		convertScaleAbs(sobelXImg, thresSobelXImg);
		cv::threshold(thresSobelXImg, thresSobelXImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//	spatialOstu(thresSobelXImg, 0.7, 4, 1);
		cv::imshow("premorph", thresSobelXImg);
		cv::morphologyEx(thresSobelXImg, morphImg, cv::MORPH_CLOSE, element);


		//扩充边界
		cv::copyMakeBorder(morphImg, morphImg, 3, 3, 3, 3, cv::BORDER_CONSTANT, cv::Scalar(0));
		cv::copyMakeBorder(tempImg, tempImg, 3, 3, 3, 3, cv::BORDER_REFLECT);

		cv::imshow("19-3morphandBorder", morphImg);
		//删除小轮廓区域
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(morphImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		std::vector<std::vector<cv::Point>>::iterator contoursIterBegin = contours.begin();
		std::vector<cv::Point>::iterator delePoints;
		cv::Mat temgimg = morphImg.clone();
		cv::Mat affinedImg;//储存仿射变换后的图像
		int hight;
		for (int i = 0; contoursIterBegin != contours.end(); i++, contoursIterBegin++) {
			cv::RotatedRect rRect = cv::minAreaRect(*contoursIterBegin);
			if (!vertifySizesTemp(rRect)) {//临时开了新的vertifySizes函数，后期整合

				std::vector<std::vector<cv::Point>> tempContours;
				tempContours.push_back(*contoursIterBegin);
				cv::fillPoly(morphImg, tempContours, cv::Scalar(0));
				//cv::imshow("morph", morphImg);
				//cv::waitKey(0);
			}
			qDebug() << i<<"finddes";
		}


		//腐蚀图像，去除干扰点。具体效果未知，后期可测试
		
		cv::copyMakeBorder(morphImg, morphImg, 0, 0, 30, 30, cv::BORDER_CONSTANT, cv::Scalar(0));
		cv::copyMakeBorder(tempImg, tempImg, 0, 0, 30, 30, cv::BORDER_CONSTANT, cv::Scalar(0));
		//element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
		//cv::morphologyEx(morphImg, morphImg, cv::MORPH_CLOSE, element);//补缺
		//cv::findContours(morphImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);


		cv::imshow("23-1morph", morphImg);

		cv::Canny(morphImg, morphImg, 50, 90);
		cv::imshow("cnanny", morphImg);
		//cv::waitKey(0);
		cv::Mat verticalImg = morphImg.clone();//用于寻找左右边线点的图像,已扩充边界

		int imgRows = morphImg.rows;
		int imgCols = morphImg.cols;
		//提取水平线上的点，去除竖直线上的点或噪声点
		std::vector<cv::Point> upperPoints;
		std::vector<cv::Point> lowerPoints;
		int maxUpperWhiteCount = 0;
		int maxLowerWhiteCount = 0;
		for (int i = 2,k=imgRows-3; k>i; i++,k--) {//提取上下水平线上的点
			int upperFiveRowWhiteCount = 0;
			int lowerFiveRowWhiteCount = 0;
			int singleUpperRowWhiteCount = 0;	
			int singleLowerRowWhiteCount = 0;
			std::vector<cv::Point> upperTempPoints;
			std::vector<cv::Point> lowerTempPoints;
			
			for (int j = 0; j < imgCols; j++) {
				//从上往下搜索
				if (morphImg.at<uchar>(i - 2,j ) > 128) {
					upperFiveRowWhiteCount++;
					upperTempPoints.push_back(cv::Point(j, i-2));
				}
				if (morphImg.at<uchar>(i - 1,j ) > 128) {
					upperFiveRowWhiteCount++;
					upperTempPoints.push_back(cv::Point(j, i-1));
				}
				if (morphImg.at<uchar>(i, j) > 128) {
					upperFiveRowWhiteCount++;
					singleUpperRowWhiteCount++;
					upperTempPoints.push_back(cv::Point(j, i));
				}
				if (morphImg.at<uchar>(i + 1,j) > 128) {
					upperFiveRowWhiteCount++;
					upperTempPoints.push_back(cv::Point(j, i+1));
				}
				if (morphImg.at<uchar>(i + 2,j) > 128) {
					upperFiveRowWhiteCount++;
					upperTempPoints.push_back(cv::Point(j, i+2));
				}


				//从下往上搜索
				if (morphImg.at<uchar>(k - 2,j) > 128) {
					lowerFiveRowWhiteCount++;
					lowerTempPoints.push_back(cv::Point(j, k - 2));
				}
				if (morphImg.at<uchar>(k - 1,j) > 128) {
					lowerFiveRowWhiteCount++;
					lowerTempPoints.push_back(cv::Point(j, k -1));
				}
				if (morphImg.at<uchar>(k,j) > 128) {
					lowerFiveRowWhiteCount++;
					lowerTempPoints.push_back(cv::Point(j, k));
					singleLowerRowWhiteCount++;
				}
				if (morphImg.at<uchar>(k+1,j) > 128) {
					lowerFiveRowWhiteCount++;
					lowerTempPoints.push_back(cv::Point(j, k + 1));
				}
				if (morphImg.at<uchar>(k + 2,j ) > 128) {
					lowerFiveRowWhiteCount++;
					lowerTempPoints.push_back(cv::Point(j, k + 2));
				}			

			}

			if (singleUpperRowWhiteCount > 4)for (int j = 0; j < imgCols; j++)verticalImg.at<uchar>(i, j) = 0;//将垂直图的行置0，去除水平线,后期查找左右边缘线用
			if (singleLowerRowWhiteCount > 4)for (int j = 0; j < imgCols; j++)verticalImg.at<uchar>(k, j) = 0;
	
																										 
																										 //上边点集判断
			if (upperFiveRowWhiteCount < 20) {

				for (int j = 0; j < imgCols; j++)morphImg.at<uchar>(i,j) = 0;//将canny图中五行行白点数小于20的中间行置0，其实这一行可以不要,
	

			}
			else if (upperFiveRowWhiteCount > maxUpperWhiteCount) {
				maxUpperWhiteCount = upperFiveRowWhiteCount;				
				upperPoints = upperTempPoints;			
				upperTempPoints.clear();

			}
			//下边点集判断
			if (lowerFiveRowWhiteCount < 20) {//以30为阈值，若遇到直线极为倾斜的状态，在阈值应降低才能找到。 20为降低阈值的情况

				for (int j = 0; j < imgCols; j++)morphImg.at<uchar>(k, j) = 0;//将canny图中三行行白点数小于20的中间行置0，其实这一行可以不要,


			}
			else if (lowerFiveRowWhiteCount > maxLowerWhiteCount) {
				maxLowerWhiteCount = lowerFiveRowWhiteCount;
				lowerPoints = lowerTempPoints;
				lowerTempPoints.clear();

			}


		}//提取上下水平线上的点

		 //提取左右边线的点，得到两个点集
		std::vector<cv::Point>  rightPoints;
		std::vector<cv::Point>  leftPoints;
		int maxLeftWhiteCount = 0;//记录从左搜索到的最大白点数
		int maxRightWhiteCount = 0;//记录从右搜索到的最大白点数
		//分别从左右开始搜索列的白点数
		for (int i = 29, k = imgCols - 29; k>i; i++, k--) {//后期改为直方图判断方式，而不以空间位置作判断
			int leftWhiteCount = 0;
			int rightWhiteCount = 0;
			std::vector<cv::Point> leftTempPoints;
			std::vector<cv::Point> rightTempPoints;

			for (int j = 2; j < imgRows - 2; j++) {

				//从左边遍历行，得到相邻五列的白点数
				if (verticalImg.at<uchar>(j - 2, i) > 128) {
					leftWhiteCount++;
					leftTempPoints.push_back(cv::Point(i, j - 2));
				}
				if (verticalImg.at<uchar>(j - 1, i) > 128) {
					leftWhiteCount++;
					leftTempPoints.push_back(cv::Point(i, j - 1));
				}
				if (verticalImg.at<uchar>(j, i) > 128) {
					leftWhiteCount++;
					leftTempPoints.push_back(cv::Point(i, j));
				}
				if (verticalImg.at<uchar>(j + 1, i) > 128) {
					leftWhiteCount++;
					leftTempPoints.push_back(cv::Point(i, j + 1));
				}
				if (verticalImg.at<uchar>(j + 2, i) > 128) {
					leftWhiteCount++;
					leftTempPoints.push_back(cv::Point(i, j + 2));
				}

				//从右边遍历行，得到相邻五列的
				if (verticalImg.at<uchar>(j - 2, k) > 128) {
					rightWhiteCount++;
					rightTempPoints.push_back(cv::Point(k, j - 2));
				}
				if (verticalImg.at<uchar>(j - 1, k) > 128) {
					rightWhiteCount++;
					rightTempPoints.push_back(cv::Point(k, j - 1));
				}
				if (verticalImg.at<uchar>(j, k) > 128) {
					rightWhiteCount++;
					rightTempPoints.push_back(cv::Point(k, j));
				}
				if (verticalImg.at<uchar>(j + 1, k) > 128) {
					rightWhiteCount++;
					rightTempPoints.push_back(cv::Point(k, j + 1));
				}
				if (verticalImg.at<uchar>(j + 2, k) > 128) {
					rightWhiteCount++;
					rightTempPoints.push_back(cv::Point(k, j + 2));
				}
			}

			if (leftWhiteCount >6 && leftWhiteCount > maxLeftWhiteCount) {
				leftPoints = leftTempPoints;
				maxLeftWhiteCount = leftWhiteCount;
			}
			if (rightWhiteCount > 6 && rightWhiteCount > maxRightWhiteCount)
			{
				rightPoints = rightTempPoints;
				maxRightWhiteCount = rightWhiteCount;
			}
			if (rightPoints.size() > 40 && leftPoints.size() > 40)break;
		}

#ifdef DEBUG //在左右边线画点
		if(0){
		for (int i = 0; i < leftPoints.size(); i++) {
			cv::circle(verticalImg, leftPoints[i], 7, cv::Scalar(128), 1);
		}
		for (int i = 0; i < rightPoints.size(); i++) {
			cv::circle(verticalImg, rightPoints[i], 15, cv::Scalar(255), 1);
		}
		cv::imshow("horizontal", morphImg);
		cv::imshow("vertical", verticalImg);
		}
#endif // DEBUG


		//判断点集状态，设置失败原因
		if (upperPoints.size() < 6 || lowerPoints.size() < 6) { 
			failure = failure | 0x01; 
			/*
			if (iterBegin == currCPlates.begin()) {
				iterBegin = currCPlates.erase(iterBegin);
				iterBegin = currCPlates.begin();
				continue;
			}
			else
			{
				iterBegin = currCPlates.erase(iterBegin);
				continue;
			}*/
		}
		if (leftPoints.size() < 10 || rightPoints.size() < 10) {
			failure = failure | 0x02; 
			/*
			if (iterBegin == currCPlates.begin()) {
				iterBegin = currCPlates.erase(iterBegin);
				iterBegin = currCPlates.begin();
				continue;
			}
			else
			{
				iterBegin = currCPlates.erase(iterBegin);
				continue;
			}*/
		}
		//由上下点集拟合水平线
		cv::Vec4f upperLine;
		cv::Vec4f lowerLine;
		cv::Vec4f leftLine;
		cv::Vec4f rightLine;
		std::vector<cv::Point> srcPoints;
		std::vector<cv::Point> dstPoints;
		if (failure == 0) {//
			//拟合直线
			cv::fitLine(upperPoints, upperLine, cv::DIST_HUBER, 0, 0.01, 0.01);
			cv::fitLine(lowerPoints, lowerLine, cv::DIST_HUBER, 0, 0.01, 0.01);
			cv::fitLine(leftPoints, leftLine, cv::DIST_HUBER, 0, 0.01, 0.01);
			cv::fitLine(rightPoints, rightLine, cv::DIST_HUBER, 0, 0.01, 0.01);

			//求斜率,并矫正偏差过大的斜率，修改直线。
			float k1 = (upperLine[1] / upperLine[0] + lowerLine[1] / lowerLine[0]) / 2;
			upperLine[1] = upperLine[0] * k1;
			lowerLine[1] = lowerLine[0] * k1;
			float k2 = (leftLine[1] / leftLine[0] + rightLine[1] / rightLine[0]) / 2;
			leftLine[1] = leftLine[0] * k2;
			rightLine[1] = rightLine[0] * k2;
			if (k1 > 1) {//斜率过大
				//failure = failure & 0x01;
				if (iterBegin == currCPlates.begin()) {
					iterBegin = currCPlates.erase(iterBegin);
					iterBegin = currCPlates.begin();
					continue;
				}
				else
				{
					iterBegin = currCPlates.erase(iterBegin);
					continue;
				}
				
			}
			if (k2 > -1 && k2 < 1) {
				//failure = failure & 0x01;
				if (iterBegin == currCPlates.begin()) {
					iterBegin = currCPlates.erase(iterBegin);
					iterBegin = currCPlates.begin();
					continue;
				}
				else
				{

					iterBegin = currCPlates.erase(iterBegin);
					continue;
				}
			}

			#ifdef DEBUG
			if(0){
			//绘制上下直线
			cv::Point upLeftPoint(0, -upperLine[2] * k1 + upperLine[3]);
			cv::Point upRightPoint(200, (200 - upperLine[2]) *k1 + upperLine[3]);
			cv::Point lowLeftPoint(0, -lowerLine[2] * k1 + lowerLine[3] + 1);
			cv::Point lowRightPoint(200, (200 - lowerLine[2]) *k1 + lowerLine[3] + 1);
			cv::line(tempImg, upLeftPoint, upRightPoint, cv::Scalar(0, 0, 255), 1);
			cv::line(tempImg, lowLeftPoint, lowRightPoint, cv::Scalar(0, 255, 255), 1);

			//绘制左右直线
			cv::Point leftPoint1(leftLine[2] - leftLine[3] / k2, 0);
			cv::Point leftPoint2((42 - leftLine[3]) / k2 + leftLine[2], 42);
			cv::Point rightPoint1(rightLine[2] - rightLine[3] / k2, 0);
			cv::Point rightPoint2((42 - rightLine[3]) / k2 + rightLine[2], 42);
			cv::circle(tempImg, leftPoint1, 4, cv::Scalar(255, 255, 0));
			cv::circle(tempImg, leftPoint2, 4, cv::Scalar(255, 255, 0));
			cv::circle(tempImg, rightPoint1, 4, cv::Scalar(255, 255, 0));
			cv::circle(tempImg, rightPoint2, 4, cv::Scalar(255, 255, 0));
			cv::line(tempImg, leftPoint1, leftPoint2, cv::Scalar(0, 0, 255), 1);
			cv::line(tempImg, rightPoint1, rightPoint2, cv::Scalar(0, 255, 255), 1);
			#endif // DEBUG
			}

			//根据直线交点进行仿射变换
			cv::Point leftTopPoint = util::getCrossPoint(upperLine, leftLine);
			cv::Point leftBottomPoint = util::getCrossPoint(lowerLine, leftLine, 0, 2);
			cv::Point rightBottomPoint = util::getCrossPoint(lowerLine, rightLine, 0, 2);
			leftTopPoint.x += 3;
			//rightBottomPoint.x += 2;
			leftBottomPoint.x += 3;
			std::vector<cv::Point2f> srcPoints;
			std::vector<cv::Point2f> dstPoints;
			srcPoints.push_back(leftTopPoint);
			srcPoints.push_back(leftBottomPoint);
			srcPoints.push_back(rightBottomPoint);
			#ifdef DEBUG
			if (0) {
				cv::circle(tempImg, leftTopPoint, 4, cv::Scalar(255, 255, 0));
				cv::circle(tempImg, leftBottomPoint, 4, cv::Scalar(255, 255, 0));
				cv::circle(tempImg, rightBottomPoint, 4, cv::Scalar(255, 255, 0));
				cv::imshow("tempImg", tempImg);	
				cv::waitKey(0);
			}
			#endif // !DEBUG
			dstPoints.push_back(cv::Point(0, 0));
			dstPoints.push_back(cv::Point(0, 36));
			dstPoints.push_back(cv::Point(136, 36));
			cv::Mat transformMat = cv::getAffineTransform(srcPoints, dstPoints);

			cv::warpAffine(tempImg, affinedImg, transformMat, cv::Size(136, 36),CV_INTER_CUBIC);
			cPlate->plateImg = affinedImg;
			cv::warpAffine(verticalImg, affinedImg, transformMat, cv::Size(136, 36), CV_INTER_CUBIC);
			cPlate->plateBinaryImg = affinedImg;
			cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "-" + std::to_string(i)+".jpg", affinedImg);
			cv::imshow("warpAffine", affinedImg);
			iterBegin++;
		}
		else
		{
			if (iterBegin == currCPlates.begin()) {
				iterBegin = currCPlates.erase(iterBegin);
				iterBegin = currCPlates.begin();
				continue;
			}
			else
			{
				iterBegin = currCPlates.erase(iterBegin);
			}
		}
		
		i++;
	}
	return 0;
}

void PlateLocate::sobelRefineResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects) {

		
	}



	void PlateLocate::sobelSecResearch(const cv::Mat &srcImg, const cv::Mat &inputImg, std::vector<CPlate>&currCPlates) {
	

		int gridWidth = inputImg.size().width / DEAFAULT_GRID_X;
		int gridx = DEAFAULT_GRID_X;
		int gridy = DEAFAULT_GRID_Y;
		uchar startGridX = 0;
		uchar endGridX = 0;
		std::vector<cv::Mat> imgPart;


		

		cv::Mat smallElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 3));
		cv::Mat bigElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(23, 5));
					
				cv::Mat tempImg1 = inputImg.clone();
				cv::Mat tempImg = tempImg1.clone();
				cv::Mat tempImg2 = tempImg1.clone();
				cv::Mat resultImg = srcImg.clone();		
		
				cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
				cv::morphologyEx(tempImg, tempImg, cv::MORPH_OPEN, element2);
				

				spatialOstu(tempImg, 0.7, gridx, gridy);
				spatialOstu(tempImg2, 0.7, gridx, gridy);
				cv::imshow("smallOpenImg", tempImg);
				//cv::waitKey(0);

				cv::Mat smallMorph;
				cv::morphologyEx(tempImg2, smallMorph, cv::MORPH_CLOSE, smallElement);
				cv::Mat bigMorph;
				cv::morphologyEx(tempImg, bigMorph, cv::MORPH_CLOSE, bigElement);
				cv::imshow("smallMorph", smallMorph);
				cv::imshow("bigMorph", bigMorph);
				//cv::waitKey(0);
				std::vector<std::vector<cv::Point>> smallContours;
				std::vector<std::vector<cv::Point>> bigContours;
				cv::findContours(smallMorph, smallContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				cv::findContours(bigMorph, bigContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				std::vector<std::vector<cv::Point>>::iterator smallItBegin = smallContours.begin();
				std::vector<std::vector<cv::Point>>::iterator bigItBegin = bigContours.begin();

				for (int i = 0; bigItBegin != bigContours.end(); bigItBegin++, i++) {
					cv::RotatedRect rotatedRect = cv::minAreaRect(*bigItBegin);
					
					if (vertifySizes(rotatedRect, DEAFAULT_SIZE_ERROR, 0)) {

						cv::Point2f* vertices = new cv::Point2f[4];
						rotatedRect.points(vertices);
						cv::Mat rRectArea;
						cv::Mat transMat;
						util::getRotatedRectArea(resultImg, rotatedRect, rRectArea,transMat);
						//cv::imshow("rotateArea", rRectArea);

#ifdef DEBUG    //画出符合条件的矩形框并存储图像，后期作车牌判断图像			
						std::string path = DEAFAULT_TEMPIMG + std::to_string(Controler::getControler()->getCurrImgCount()) + "_" + std::to_string(i) + ".jpg";
						//					qDebug() << cv::imwrite(path, rRectArea);//如果写入成功将输出true
											//画旋转矩形
						std::vector<cv::Point>rectcontour;
						for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
						std::vector<std::vector<cv::Point>> rectcontous;
						rectcontous.push_back(rectcontour);
						//cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(255, 0, 0), 1);//改动,blue
#endif // DEBUG		

						cv::imshow("secproposalPlate", rRectArea);
//sobelFrtRects.push_back(rotatedRect);//存储判定为车牌区域的旋转矩形
						float score;
						if (svm.startJudge(rRectArea,score)) {
							CPlate tempPlate;
							//绘制判断为车牌的矩形
							cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 255, 0), 1);//改动,green
							tempPlate.srcSize = currSize;
							tempPlate.plateImg=rRectArea;//考虑第三次搜索完毕再存储
							tempPlate.setPlateRotaRect(rotatedRect);
							currCPlates.push_back(tempPlate);
							platecout++;
							cv::Mat transMat;
							util::getRotatedRectArea(srcImg, rotatedRect, rRectArea, transMat, true);
							//cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "_"+std::to_string(i) + ".jpg", rRectArea);
							cv::imshow("proposalPlate", rRectArea);
							//cv::waitKey(0);
							cv::Mat gridImg;
							util::drawGrid(resultImg, gridImg, DEAFAULT_GRID_X, DEAFAULT_GRID_Y);
							Controler::getControler()->showImg(gridImg);
						}
				//		cv::waitKey(0);
					}
				}



				resultImg = srcImg.clone();
				for (int i = 0; smallItBegin != smallContours.end(); smallItBegin++, i++) {
					cv::RotatedRect rotatedRect = cv::minAreaRect(*smallItBegin);
					if (vertifySizes(rotatedRect, DEAFAULT_SIZE_ERROR, 0)) {

						cv::Point2f* vertices = new cv::Point2f[4];
						rotatedRect.points(vertices);
						cv::Mat rRectArea;
						cv::Mat transMat;
						util::getRotatedRectArea(resultImg, rotatedRect, rRectArea, transMat);
						//					cv::imshow("rotateArea", rRectArea);
											//cv::waitKey(0);
#ifdef DEBUG    //画出符合条件的矩形框并存储图像，后期作车牌判断图像			
						std::string path = DEAFAULT_TEMPIMG + std::to_string(Controler::getControler()->getCurrImgCount()) + "___" + std::to_string(i) + ".jpg";
						//					qDebug() << cv::imwrite(path, rRectArea);//如果写入成功将输出true
											//画旋转矩形
						std::vector<cv::Point>rectcontour;
						for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
						std::vector<std::vector<cv::Point>> rectcontous;
						rectcontous.push_back(rectcontour);
						cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 255,  255), 1);//改动
#endif // DEBUG	

						cv::imshow("secproposalPlate", rRectArea);
//sobelFrtRects.push_back(rotatedRect);//存储判定为车牌区域的旋转矩形
						float score;
						if (svm.startJudge(rRectArea, score)) {
							CPlate tempPlate;
							//绘制判断为车牌的矩形
							cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(255, 255, 255), 1);//改动
							tempPlate.srcSize = currSize;
							tempPlate.plateImg=rRectArea;//考虑第三次搜索完毕再存储
							tempPlate.setPlateRotaRect(rotatedRect);
							currCPlates.push_back(tempPlate);
							platecout++;
							cv::Mat transMat;
							util::getRotatedRectArea(srcImg, rotatedRect, rRectArea, transMat ,true);
							//cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "___" + std::to_string(i) + ".jpg", rRectArea);
							cv::imshow("proposalPlate", rRectArea);
							//cv::waitKey(0);
						}
				//		cv::waitKey(0);
						Controler::getControler()->showImg(resultImg);
					}
				}


			

			
						
		
	}


	void PlateLocate::sobelFrtResearch(const cv::Mat &srcImg, const cv::Mat &proceSobelImg, std::vector<CPlate>&currCPlates) {	
		cv::Mat sobelImg = proceSobelImg.clone();
		cv::Mat resultImg = srcImg.clone();
		cv::Point2f rectPoints[4];//存储矩形的四个点
		cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 3));		
		
		cv::Mat sobelMorph;

		
		//cv::morphologyEx(thresholdImg, sobelMorph, cv::MORPH_CLOSE, element);

		//SHOW_IMAGE(sobelMorph, 1);//新增
		std::vector<std::vector<cv::Point>> contours;
		cv::RotatedRect rotatedRect;
		cv::findContours(sobelMorph, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		std::vector<std::vector<cv::Point>>::iterator it = contours.begin();



		for (int i = 0; it != contours.end(); it++, i++) {
			rotatedRect = cv::minAreaRect(cv::Mat(*it));
			if (vertifySizes(rotatedRect, DEAFAULT_SIZE_ERROR, 0)) {

				cv::Point2f* vertices = new cv::Point2f[4];
				rotatedRect.points(vertices);
				cv::Mat rRectArea;
				cv::Mat transMat;
				util::getRotatedRectArea(resultImg, rotatedRect, rRectArea, transMat);


#ifdef DEBUG    //画出符合条件的矩形框并存储图像，后期作车牌判断图像			
				std::string path = DEAFAULT_TEMPIMG + std::to_string(Controler::getControler()->getCurrImgCount()) + std::to_string(i) + ".jpg";
				//			qDebug() << cv::imwrite(path, rRectArea);//如果写入成功将输出true
							//画旋转矩形
				std::vector<cv::Point>rectcontour;
				for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
				std::vector<std::vector<cv::Point>> rectcontous;
				rectcontous.push_back(rectcontour);
				cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(255, 255, 0), 1);//改动
#endif // DEBUG	

				float score;
				if (svm.startJudge(rRectArea,score)) {
					//绘制判断为车牌的矩形
					cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 0, 255), 1);//改动
					CPlate tempPlate;
					tempPlate.srcSize = currSize;
					tempPlate.setPlateRotaRect(rotatedRect);
					platecout++;
					cv::Mat transMat;
					util::getRotatedRectArea(srcImg, rotatedRect, rRectArea, transMat, true);
					//cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "-" + std::to_string(i) + ".jpg", rRectArea);
					//Controler::getControler()->showImg(plateImg);
					tempPlate.plateImg = rRectArea;//存储扩大后的矩形区域
					currCPlates.push_back(tempPlate);				
				}
			}
		}

		Controler::getControler()->showImg(resultImg);//显示在sobel图上找到的可能的矩形
		//getImgPoint(inputImg, "sobel");
	}

	//计算图像跳点,阈值为单行平均跳点数
	bool  PlateLocate::jumpPointCount(const cv::Mat plateImg,uchar threshold) {
		cv::imshow("plateImg", plateImg);
//		cv::waitKey(0);
		if (plateImg.rows < 6)return false;
		int mid = plateImg.rows /2;
		const uchar* inData0 = plateImg.ptr<uchar>(mid-2);
		const uchar* inData1 = plateImg.ptr<uchar>(mid);		
		const uchar* inData2 = plateImg.ptr<uchar>(mid+2);
		int jumpCount=0;
		for (int i = 0; i < (plateImg.cols-1); i++) {
			if (inData0[i] != inData0[i + 1])jumpCount++;
			if (inData1[i] != inData1[i + 1])jumpCount++;
			if (inData2[i] != inData2[i + 1])jumpCount++;
		}
		qDebug() << jumpCount/3;
		if (jumpCount/3 > threshold)return true;
		return false;
	}



/**
 * @brief sobel第一次搜索
 * detail description
 * @param srcImg	 输入图像，process过的图像
 * @param inputImg    输入图像
 * @param sobelFrtRects    储存判定为车牌区域的旋转矩形
 * @param hitArea    储存含有车牌的栅格信息 
 */
void PlateLocate::sobelFrtResearch1(const cv::Mat &srcImg,const cv::Mat &grayImg,const cv::Mat &inputImg, std::vector<CPlate>&currCPlates) {
	
	int gridx = DEAFAULT_GRID_X, gridy = DEAFAULT_GRID_Y;
	
	cv::Point2f rectPoints[4];//存储矩形的四个点

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 3));
	cv::Mat thresholdImg = inputImg.clone();
	cv::Mat resultImg= PlateLocate::srcImg.clone();
	cv::Mat sobelMorph;
//	cv::imshow("sobel", inputImg);
//	spatialOstu(thresholdImg, 0.7,1, 1);//分为6*8个栅格，效果最佳//考虑要不要与其他sobelsearch函数共享
//	cv::threshold(thresholdImg, thresholdImg, 255, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);

//	cv::imshow("sobelThreshold", thresholdImg);
	if(sizeI>0.6){
	cv::Mat element2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 2));
	cv::Mat open;
	cv::morphologyEx(thresholdImg, thresholdImg, cv::MORPH_OPEN, element2);
//	cv::imshow("tbreadholdOpen", thresholdImg);
	}

	
	//cv::waitKey(0);
//	cv::imshow("ostuImg", thresholdImg);
	cv::morphologyEx(thresholdImg, sobelMorph, cv::MORPH_CLOSE, element);

//	cv::imshow("morphAfterOpen", sobelMorph);
//	cv::waitKey(0);

	//SHOW_IMAGE(sobelMorph, 1);//新增
	std::vector<std::vector<cv::Point>> contours;
	cv::RotatedRect rotatedRect;
	cv::findContours(sobelMorph, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator it = contours.begin();
	
	
	
	for (int i = 0; it != contours.end(); it++, i++) {
		rotatedRect = cv::minAreaRect(cv::Mat(*it));

		
	//	cv::RotatedRect rrect = rotatedRect;
	//	rrect.size.width *= 1.2;
	//	cv::Rect2f rect;
	//	util::calcSafeRect(rrect, srcImg.size(), rect);

		

		if (vertifySizes(rotatedRect, 0.6, 0)) {
			
			rotatedRect.center = rotatedRect.center / sizeI;
			rotatedRect.size.width = rotatedRect.size.width / sizeI;
			rotatedRect.size.height = rotatedRect.size.height / sizeI;
			

			cv::Rect2f rect;
			util::calcSafeRect(rotatedRect, grayImg.size(), rect);			
			cv::Mat rectImg(grayImg, rect);
			cv::Mat rectImg2=rectImg.clone();
		//	spatialOstu(rectImg2, 0.7, 3, 1);
			cv::threshold(rectImg, rectImg2, 255, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);
		//	cv::Canny(rectImg2, rectImg2, 20, 85);
			cv::imshow("jumpPointImg", rectImg2);
		//	cv::waitKey(0);
			if (!jumpPointCount(rectImg2,8))continue;
			

			cv::Point2f* vertices = new cv::Point2f[4];
			rotatedRect.points(vertices);
			cv::Mat rRectArea;
			cv::Mat transMat;
			cv::RotatedRect temprrect = rotatedRect;
			util::getRotatedRectArea(resultImg, rotatedRect, rRectArea, transMat);
			cv::imshow("firstProposalArea",rRectArea);
		//	cv::waitKey(0);
			#ifdef DEBUG    //画出符合条件的矩形框并存储图像，后期作车牌判断图像			
			std::string path = DEAFAULT_TEMPIMG + std::to_string(Controler::getControler()->getCurrImgCount()) + std::to_string(i) + ".jpg";
//			qDebug() << cv::imwrite(path, rRectArea);//如果写入成功将输出true
			//画旋转矩形
			std::vector<cv::Point>rectcontour;
			for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
			std::vector<std::vector<cv::Point>> rectcontous;
			rectcontous.push_back(rectcontour);
		//	cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(255, 255, 0), 1);//改动
			#endif // DEBUG	
		//	cv::cvtColor(rRectArea, rRectArea, CV_BGR2GRAY);
		//	clahe->apply(rRectArea, rRectArea);
		//	cv::threshold(rRectArea, rRectArea, 255, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
			
		//	cv::imshow("test", rRectArea);
		//	cv::waitKey(0);


			float score=0;
			if(svm.startJudge(rRectArea,score)){

			//绘制判断为车牌的矩形
		//	cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 0, 255), 1);//改动
			if (currCPlates.size()==0) {
		//	if (1) {
				CPlate tempPlate;
				tempPlate.srcSize = currSize;
				//	tempPlate.setPlateRotaRect(rotatedRect);
				tempPlate.plateRotaRect = rotatedRect;
				cv::RotatedRect temprrect = rotatedRect;
				cv::Mat transMat;
				util::getRotatedRectArea(resultImg, tempPlate.plateRotaRect, rRectArea, transMat,true);
				qDebug() <<"mat:"<<transMat.data<<"type"<< transMat.type();
			//	cv::resize(rRectArea, rRectArea, cv::Size(136, 36));
				//cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) +"-"+ std::to_string(i) + ".jpg", rRectArea);
				//Controler::getControler()->showImg(plateImg);
				tempPlate.plateImg = rRectArea;//存储扩大后的矩形区域
				tempPlate.setScore(score);
				//transMat.
				cv::Mat num=(cv::Mat_<double>(1, 3) << 0, 0, 1);
				transMat.push_back(num);			

				tempPlate.transformMat = transMat.clone();
				tempPlate.srcImg = srcImg;
				currCPlates.push_back(tempPlate);
				cv::imshow("RotatedRect  size0", rRectArea);
		//		cv::waitKey(0);
			}
			else
			{
				std::vector<CPlate>::iterator itBegin=currCPlates.begin();
				bool isAdd=true;
				cv::imshow("candidate plate area", rRectArea);
				while ( itBegin != currCPlates.end()) {
					int width=0;
					int height = 0;
					if (rotatedRect.size.width < rotatedRect.size.height) {
						 width = rotatedRect.size.height;
						 height = rotatedRect.size.width;
					}
					else
					{
						width= rotatedRect.size.width;
						height=rotatedRect.size.height;

					}
						if ((std::abs(itBegin->plateRotaRect.center.x - rotatedRect.center.x )< width) &&
							(std::abs(itBegin->plateRotaRect.center.y - rotatedRect.center.y) < height)) {
							
							if (itBegin->getScore() < -0.5&& score < -0.5) {
								break;
							}

							if (itBegin->getScore() < score) {
								cv::imshow("low score old Plate Area", itBegin->plateImg);
							//	cv::waitKey(0);
								isAdd = false;
								itBegin++;
								break; 
							}
							else
							{
								cv::imshow("hight score old Plate Area", itBegin->plateImg);
								itBegin=currCPlates.erase(itBegin);
							//	cv::waitKey(0);
							//	isAdd = true;
							}
						}
						else
						{
							itBegin++;
						}
					
					//cv::waitKey(0);

				}
				if (isAdd) {

					
					CPlate tempPlate;
					tempPlate.srcSize = currSize;
					//	tempPlate.setPlateRotaRect(rotatedRect);
					tempPlate.plateRotaRect = rotatedRect;
					cv::Mat transformMat;
					cv::RotatedRect temprrect = rotatedRect;
					util::getRotatedRectArea(srcImg, tempPlate.plateRotaRect, rRectArea, transformMat,true);

					//cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) +"-"+ std::to_string(i) + ".jpg", rRectArea);
					//Controler::getControler()->showImg(plateImg);
					tempPlate.plateImg = rRectArea;//存储扩大后的矩形区域
					tempPlate.setScore(score);

					cv::Mat num = (cv::Mat_<double>(1, 3) << 0, 0, 1);
					transMat.push_back(num);

					tempPlate.transformMat = transMat.clone();
					tempPlate.srcImg = srcImg;
					currCPlates.push_back(tempPlate);
					cv::imshow("RotatedRect", rRectArea);
			//		cv::waitKey(0);
				}

			}
			
			
			
			}
		//	cv::waitKey(0);
			}
		
		}
		
	
	
//	Controler::getControler()->showImg(resultImg);//显示在sobel图上找到的可能的矩形
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
bool PlateLocate::vertifySizes(const cv::RotatedRect &rRect,float error,uchar mode) {
	
	float mError = error;//长宽比偏差值

	float ratioMin =DEAFAULT_SIZE_ASPECT-DEAFAULT_SIZE_ASPECT * (mError);//计算得到最小比例
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