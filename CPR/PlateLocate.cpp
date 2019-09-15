#include"stdafx.h"
#pragma once
#include "PlateLocate.h"
#include "Controler.h"
#include<util.h>

#include"CNNRecognizer.h"
#include"CPlate.h"
#include"CharsSegment.h"
//extern Controler *controler;
#define DEBUG 1
//macro config
//#define IDEA_WIDTH 1200
//#define IDEA_HIGHT 600

#define DEAFAULT_TEMPIMG "D:\\VS\\CPR\\TEMPIMG\\"
PlateLocate::PlateLocate()
{

}

PlateLocate::~PlateLocate()
{
}


///int PlateLocate::platecout = 0;
std::string PlateLocate::tempPath = "D:/VS/CPR/recoPlate/";//识别车牌的保持路径


void batchTest(std::vector<std::string> vecPath) {

}

/**
 * @brief 车牌定位
 * 车牌定位的入口
 * @param srcImg      原图
 * @param currCPlate     当前车牌信息
 *
 * @return 是否定位成功
 *     -<em>false</em> 定位成功
 *     -<em>true</em> 定位失败
 */
bool PlateLocate::locate(cv::Mat srcImg,std::vector<CPlate> &currCPlates) {
	currSize = srcImg.size();
	cv::Mat tempImg;
	preProcess(srcImg, tempImg);

	Controler::getControler()->showImg(tempImg);
	std::vector<cv::RotatedRect> rotatedRects;
	//sobelResearch(tempImg, rotatedRects);//改动
	sobelResearch(tempImg, rotatedRects,currCPlates);
	
	/*
	if (rotatedRects.size() == 0) {
		//cv::imwrite(path + "false.jpg",srcImg);
		currPlates.clear();
		return false;//定位失败，后期可设置特定标志位

	}*/

	//plateJudge(srcImg, rotatedRects);

	if (currCPlates.empty()) {
		//cv::imwrite(path + "false.jpg", srcImg);
		return false;//如果识别的车牌数为空，返回false
	}

	/*
	if (currPlates.plateImgs.empty())return 0;//K均值聚类分割图像，效果不佳，暂时不用
	kMean(currPlates.plateImgs[0], cv::Size(3, 3), 3);
	currPlates.plateImgs.clear();
	*/	
		
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
	

	
	//cv::waitKey(0);
	/*
	cv::Mat blurImg;
	//cv::GaussianBlur(srcImg, blurImg, blurSize, 0);

	cv::Mat sizeImg;
	cv::resize(srcImg, sizeImg, cv::Size(390,118));
	//cv::cvtColor(sizeImg, sizeImg, CV_BGR2GRAY);
	SHOW_IMAGE(sizeImg, 1);
	//cv::ga
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


void PlateLocate::findMappingVertical(const cv::Mat &srcImg, cv::Mat &outputImg) {
	cv::Mat sizeImg;
	cv::resize(srcImg, sizeImg, cv::Size(136, 36));
	cv::Mat thresImg;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Rect> rects;
	cv::Rect rect;
	std::vector<cv::Point> upPoints;
	std::vector<cv::Point> lowPoints;
	char upper = 10;
	char lowper = -12;
	char step = 2;
	for (; lowper < upper; lowper += step) {
		util::AdaptiveThereshold(sizeImg, thresImg, lowper);
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
			if (vertifyCharacterSizes(rect)) {
				upPoints.push_back(rect.tl());
				lowPoints.push_back(rect.br());
				cv::Mat debugImg2 = sizeImg.clone();
				cv::rectangle(debugImg2, rect, cv::Scalar(255, 255, 255), 2);

				cv::imshow("rectangle", debugImg2);
				cv::waitKey(0);
			}
			else
			{
				cv::Mat badImg = sizeImg.clone();
				cv::rectangle(badImg, rect, cv::Scalar(255, 255, 255), 2);
				cv::resize(badImg, badImg, cv::Size(408, 108));
				cv::imshow("badImg", badImg);
				cv::waitKey(0);
			}
		}
	}
	if (upPoints.size() < 10 && lowPoints.size() < 5) {
		lowper = -14;
		upper = 10;
		for (; lowper < upper; lowper += step) {
			util::AdaptiveThereshold(sizeImg, thresImg, lowper);
			cv::threshold(thresImg, thresImg, 126, 255, CV_THRESH_BINARY_INV);
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
				if (vertifyCharacterSizes(rect)) {
					upPoints.push_back(rect.tl());
					lowPoints.push_back(rect.br());
					cv::Mat debugImg2 = sizeImg.clone();
					cv::rectangle(debugImg2, rect, cv::Scalar(255, 255, 255), 2);

					cv::imshow("rectangle", debugImg2);
					cv::waitKey(0);
				}
				else
				{
					cv::Mat badImg = sizeImg.clone();
					cv::rectangle(badImg, rect, cv::Scalar(255, 255, 255), 2);
					cv::resize(badImg, badImg, cv::Size(408, 108));
					cv::imshow("badImg", badImg);
					cv::waitKey(0);
				}
			}
		}
	}
	if (upPoints.size() < 3 || lowPoints.size() < 3)return;
	cv::Vec4f line;
	cv::fitLine(upPoints, line, cv::DIST_HUBER, 0, 0.01, 0.01);
	float vx = line[0];
	float vy = line[1];
	float x = line[2];
	float y = line[3];

	//实际上求直线x=0与y=kx+c相交时的y1值，以（0，y1）作为边线左端的坐标
	int lefty = static_cast<int>((-x * vy / vx) + y);
	//实际上是求当x轴平移136个像素时，直线x=136与y轴相交的y2值，以（136，y2）作为边线右端的坐标
	int righty = static_cast<int>(((136 - x) * vy / vx) + y);
	cv::line(sizeImg, cv::Point(0, lefty), cv::Point(136, righty), cv::Scalar(255, 255, 255), 1);


	cv::fitLine(lowPoints, line, cv::DIST_HUBER, 0, 0.01, 0.01);
	vx = line[0];
	vy = line[1];
	x = line[2];
	y = line[3];

	//实际上求直线x=0与y=kx+c相交时的y1值，以（0，y1）作为边线左端的坐标
	lefty = static_cast<int>((-x * vy / vx) + y);
	//实际上是求当x轴平移136个像素时，直线x=136与y轴相交的y2值，以（136，y2）作为边线右端的坐标
	righty = static_cast<int>(((136 - x) * vy / vx) + y);
	cv::line(sizeImg, cv::Point(0, lefty), cv::Point(136, righty), cv::Scalar(255, 255, 255), 1);

	cv::imshow("ddd", sizeImg);
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
			if (cellThreshVal<globalThreshVal*persent) {//再加上全局阈值的判断
				cv::threshold(src_cell, src_cell, 255, 255,  CV_THRESH_BINARY);
			}
			else
			{
				cv::threshold(src_cell, src_cell, 77, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
			}
			
			//cv::imshow("after src_cell", src_cell);
			//cv::waitKey(0);
		}
	}
}


//判断矩形是否是字符区域的矩形
bool PlateLocate::vertifyCharacterSizes(const cv::Rect &Rect) {
	float ratio = Rect.width / (static_cast<float>(Rect.height));
	float area = Rect.area();
	if ((ratio > 0.3 && ratio < 1.2) && (area > 130 && area < 500)) {
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
		
		
		util::getRotatedRectArea(srcImg, rotatedRects[i], plateImg,false);
		
		if (svm.startJudge(plateImg)) {
			//currPlates.plateImgs.push_back(plateImg);
			
			//currPlates.rRects.push_back(rotatedRects[i]);
			platecout++;
			util::getRotatedRectArea(srcImg, rotatedRects[i], saveImg, true);
			cv::imwrite(tempPath+std::to_string(Controler::getControler()->getCurrImgCount())+ std::to_string(i)+".jpg", saveImg);
			//Controler::getControler()->showImg(plateImg);
			
		}
		
	}
	
}


void PlateLocate::sobelResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect> &rRects,std::vector<CPlate>&currCPlates) {
	
	cv::Mat grayImg;
	cv::Mat preProceImg;//先为预处理图像，后期添加可能矩形轮廓并显示
	cv::Mat sobelImg;
	preProcess(inputImg, preProceImg);//后期应与外面的preProcess处理过程不同
	cv::cvtColor(preProceImg, grayImg, cv::COLOR_BGR2GRAY);//改动
	cv::Sobel(grayImg, sobelImg, CV_8U, 1, 0);

	bool hitArea[DEAFAULT_GRID_X];
	memset(hitArea, 0, sizeof(hitArea));
	
	sobelFrtResearch(preProceImg,sobelImg, rRects,currCPlates, hitArea);
	//memset(hitArea, 0, sizeof(hitArea));
	//if (rRects.size() <= 0)cv::imwrite("D:/VS/CPR/falseImg/" + std::to_string(Controler::getControler()->getCurrImgCount()) + ".jpg",inputImg);
	sobelSecResearch(preProceImg,sobelImg, rRects,currCPlates, hitArea);//使用sobel算子及其他形态学算子查找图片剩余区域
	//sobelRefineResearch(preProceImg, rRects);
	findDdgeDeskew(currCPlates);      

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

uchar PlateLocate::findDdgeDeskew(std::vector<CPlate>&currCPlates) {
	
	//std::vector<CPlate> extendImg = currCPlates;
	std::vector<CPlate>::iterator iterBegin = currCPlates.begin();
	cv::Mat sobelXImg;
	cv::Mat thresSobelXImg;
	cv::Mat grayImg;
	cv::Mat morphImg;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 3));
	//std::vector<cv::Vec4i> lines;
	//std::vector<cv::Vec2f> lines;
	//cv::Mat tempImg;

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
		for (int i = 0; i < leftPoints.size(); i++) {
			cv::circle(verticalImg, leftPoints[i], 7, cv::Scalar(128), 1);
		}
		for (int i = 0; i < rightPoints.size(); i++) {
			cv::circle(verticalImg, rightPoints[i], 15, cv::Scalar(255), 1);
		}
		cv::imshow("horizontal", morphImg);
		cv::imshow("vertical", verticalImg);
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
			#ifndef DEBUG
			if (0) {
				cv::circle(tempImg, leftTopPoint, 4, cv::Scalar(255, 255, 0));
				cv::circle(tempImg, leftBottomPoint, 4, cv::Scalar(255, 255, 0));
				cv::circle(tempImg, rightBottomPoint, 4, cv::Scalar(255, 255, 0));
				cv::imshow("tempImg", tempImg);		}
			#endif // !DEBUG
			dstPoints.push_back(cv::Point(0, 0));
			dstPoints.push_back(cv::Point(0, 36));
			dstPoints.push_back(cv::Point(136, 36));
			cv::Mat transformMat = cv::getAffineTransform(srcPoints, dstPoints);

			cv::warpAffine(tempImg, affinedImg, transformMat, cv::Size(136, 36),CV_INTER_CUBIC);
			cPlate->plateImg = affinedImg;
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
		/*
		CharsSegment charsSegent;
		cv::Mat plateThresImg;
		std::vector<cv::RotatedRect>rr;
		charsSegent.segmentUsingProjection(affinedImg, plateThresImg, rr);
		cPlate->plateBinaryImg = plateThresImg;
		cPlate->characterRects = rr;

			std::string caffeModel = "D:/VS/CPR/model/CharacterRecognization.caffemodel";
			std::string prototext = "D:/VS/CPR/model/CharacterRecognization.prototxt";
			CNNRecognizer *cnnRecognizer = new CNNRecognizer(prototext, caffeModel);
			std::vector<CPlate> tempVec;
			tempVec.push_back(*cPlate);
			cnnRecognizer->SegmentBasedSequenceRecognition(tempVec);

			std::string tempPath1 = "D:/VS/CPR/recoPlate1/";
		cv::imwrite(tempPath1 + std::to_string(Controler::getControler()->getCurrImgCount()) + "-2" + ".jpg", plateThresImg);
		cv::imshow("houghline", tempImg);
		//cv::waitKey(0);
		*/
	}
	return 0;
}

void PlateLocate::sobelRefineResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects) {

		
	}



	void PlateLocate::sobelSecResearch(const cv::Mat &srcImg, const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects, std::vector<CPlate>&currCPlates, bool *hitArea) {
//		SVMJudge svm;
		int gridWidth = inputImg.size().width / DEAFAULT_GRID_X;
		int gridx = DEAFAULT_GRID_X;
		int gridy = DEAFAULT_GRID_Y;
		uchar startGridX = 0;
		uchar endGridX = 0;
		std::vector<cv::Mat> imgPart;

		cv::Mat smallElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 1));
		cv::Mat bigElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(23, 5));
		for (int i = 0; i < DEAFAULT_GRID_X; i++) {

			if ((hitArea[i] == true || (i == DEAFAULT_GRID_X - 1)) && startGridX < i) {
				cv::Mat tempImg1 = cv::Mat(inputImg, cv::Range::all(), cv::Range(startGridX*gridWidth, (i + 1)*gridWidth));
				cv::Mat tempImg = tempImg1.clone();
				cv::Mat resultImg = srcImg.clone();
				//imgPart.push_back(tempImg);			

				//cv::imshow("part", tempImg);//显示图像是否正常分割
				//cv::waitKey(0);

				spatialOstu(tempImg, 0.7,2, gridy);

				cv::Mat smallMorph;
				cv::morphologyEx(tempImg, smallMorph, cv::MORPH_CLOSE, smallElement);
				cv::Mat bigMorph;
				cv::morphologyEx(tempImg, bigMorph, cv::MORPH_CLOSE, bigElement);
				//cv::imshow("smallMorph", smallMorph);
				//cv::imshow("bigMorph", bigMorph);
				//cv::waitKey(0);
				std::vector<std::vector<cv::Point>> smallContours;
				std::vector<std::vector<cv::Point>> bigContours;
				cv::findContours(smallMorph, smallContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				cv::findContours(bigMorph, bigContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
				std::vector<std::vector<cv::Point>>::iterator smallItBegin = smallContours.begin();
				std::vector<std::vector<cv::Point>>::iterator bigItBegin = bigContours.begin();

				for (int i = 0; bigItBegin != bigContours.end(); bigItBegin++, i++) {
					cv::RotatedRect rotatedRect = cv::minAreaRect(*bigItBegin);
					if (startGridX != 0)rotatedRect.center.x += startGridX * gridWidth;
					if (vertifySizes(rotatedRect, DEAFAULT_SIZE_ERROR, 0)) {

						cv::Point2f* vertices = new cv::Point2f[4];
						rotatedRect.points(vertices);
						cv::Mat rRectArea;
						util::getRotatedRectArea(resultImg, rotatedRect, rRectArea);
						//cv::imshow("rotateArea", rRectArea);

#ifdef DEBUG    //画出符合条件的矩形框并存储图像，后期作车牌判断图像			
						std::string path = DEAFAULT_TEMPIMG + std::to_string(Controler::getControler()->getCurrImgCount()) + "_" + std::to_string(i) + ".jpg";
						//					qDebug() << cv::imwrite(path, rRectArea);//如果写入成功将输出true
											//画旋转矩形
						std::vector<cv::Point>rectcontour;
						for (int i = 0; i < 4; i++)rectcontour.push_back(vertices[i]);
						std::vector<std::vector<cv::Point>> rectcontous;
						rectcontous.push_back(rectcontour);
						cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(255, 0, 0), 1);//改动
#endif // DEBUG	


//sobelFrtRects.push_back(rotatedRect);//存储判定为车牌区域的旋转矩形
						if (svm.startJudge(rRectArea)) {
							CPlate tempPlate;
							//绘制判断为车牌的矩形
							cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 255, 0), 1);//改动
							tempPlate.srcSize = currSize;
							tempPlate.plateImg=rRectArea;//考虑第三次搜索完毕再存储
							tempPlate.setPlateRotaRect(rotatedRect);
							currCPlates.push_back(tempPlate);
							platecout++;
							util::getRotatedRectArea(srcImg, rotatedRect, rRectArea, true);
							cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "_"+std::to_string(i) + ".jpg", rRectArea);

							cv::Mat gridImg;
							util::drawGrid(resultImg, gridImg, DEAFAULT_GRID_X, DEAFAULT_GRID_Y);
							Controler::getControler()->showImg(gridImg);
						}

					}
				}




				resultImg = srcImg.clone();
				for (int i = 0; smallItBegin != smallContours.end(); smallItBegin++, i++) {
					cv::RotatedRect rotatedRect = cv::minAreaRect(*smallItBegin);
					if (startGridX != 0)rotatedRect.center.x += startGridX * gridWidth;

					if (vertifySizes(rotatedRect, DEAFAULT_SIZE_ERROR, 0)) {

						cv::Point2f* vertices = new cv::Point2f[4];
						rotatedRect.points(vertices);
						cv::Mat rRectArea;
						//rotatedRect.center.x=
						util::getRotatedRectArea(resultImg, rotatedRect, rRectArea);
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
						cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 255, 255), 1);//改动
#endif // DEBUG	


//sobelFrtRects.push_back(rotatedRect);//存储判定为车牌区域的旋转矩形
						if (svm.startJudge(rRectArea)) {
							CPlate tempPlate;
							//绘制判断为车牌的矩形
							cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(255, 255, 255), 1);//改动
							tempPlate.srcSize = currSize;
							tempPlate.plateImg=rRectArea;//考虑第三次搜索完毕再存储
							tempPlate.setPlateRotaRect(rotatedRect);
							currCPlates.push_back(tempPlate);
							platecout++;
							util::getRotatedRectArea(srcImg, rotatedRect, rRectArea, true);
							cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "___" + std::to_string(i) + ".jpg", rRectArea);

						}
						cv::Mat gridImg;
						util::drawGrid(resultImg, gridImg, DEAFAULT_GRID_X, DEAFAULT_GRID_Y);
						Controler::getControler()->showImg(gridImg);
					}
				}


				startGridX = i + 1;

			}
			else if (hitArea[i] == true && startGridX == i) {
				startGridX += 1;
				qDebug() << "startGridX++";
			}
			else if (hitArea[i] != true && startGridX == i) {
				qDebug() << "how";//第i块未被击中，且开始点==i，什么都不用做
			}


		}
	}


/**
 * @brief sobel第一次搜索
 * detail description
 * @param srcImg	 输入图像，process过的图像
 * @param inputImg    输入图像
 * @param sobelFrtRects    储存判定为车牌区域的旋转矩形
 * @param hitArea    储存含有车牌的栅格信息 
 */
void PlateLocate::sobelFrtResearch(const cv::Mat &srcImg, const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelFrtRects, std::vector<CPlate>&currCPlates, bool *hitArea) {
//	SVMJudge svm;
	
	int gridx = DEAFAULT_GRID_X, gridy = DEAFAULT_GRID_Y;
	int gridXWidth= inputImg.size().width/gridx;
	
	cv::Point2f rectPoints[4];//存储矩形的四个点

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3));
	cv::Mat thresholdImg = inputImg.clone();
	cv::Mat resultImg= srcImg.clone();
	cv::Mat sobelMorph;
//	cv::imshow("sobel", inputImg);
	spatialOstu(thresholdImg, 0.7,gridx, gridy);//分为6*8个栅格，效果最佳//考虑要不要与其他sobelsearch函数共享
	//cv::threshold(tempImg, tempImg, 255, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);

	//SHOW_IMAGE(tempImg, 1);//新增
//	cv::imshow("ostuImg", thresholdImg);
	cv::morphologyEx(thresholdImg, sobelMorph, cv::MORPH_CLOSE, element);

	//SHOW_IMAGE(sobelMorph, 1);//新增
	std::vector<std::vector<cv::Point>> contours;
	cv::RotatedRect rotatedRect;
	cv::findContours(sobelMorph, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator it = contours.begin();
	
	
	
	for (int i = 0; it != contours.end(); it++, i++) {
		rotatedRect = cv::minAreaRect(cv::Mat(*it));
		if (vertifySizes(rotatedRect, DEAFAULT_SIZE_ERROR,0)) {

			cv::Point2f* vertices = new cv::Point2f[4];
			rotatedRect.points(vertices);
			cv::Mat rRectArea;
			util::getRotatedRectArea(resultImg, rotatedRect, rRectArea);
			

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
			

			sobelFrtRects.push_back(rotatedRect);//存储判定为车牌区域的旋转矩形
			if(svm.startJudge(rRectArea)){

			//绘制判断为车牌的矩形
			cv::drawContours(resultImg, rectcontous, 0, cv::Scalar(0, 0, 255), 1);//改动
			
			
			CPlate tempPlate;
			tempPlate.srcSize = currSize;
			tempPlate.setPlateRotaRect(rotatedRect);
			platecout++;
			util::getRotatedRectArea(srcImg, rotatedRect, rRectArea, true);
			cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) +"-"+ std::to_string(i) + ".jpg", rRectArea);
			//Controler::getControler()->showImg(plateImg);
			tempPlate.plateImg=rRectArea;//存储扩大后的矩形区域
			currCPlates.push_back(tempPlate);
			//判断车牌所在栅格区域			
			rotatedRect.points(rectPoints);
			int leftX= static_cast<int>(rectPoints[0].x < rectPoints[1].x ? rectPoints[0].x : rectPoints[1].x);
			int rightX=static_cast<int>(rectPoints[2].x > rectPoints[3].x ? rectPoints[2].x : rectPoints[3].x);
			int leftNum = leftX / gridXWidth;
			int rightNum = rightX / gridXWidth;
			
			if (leftNum == rightNum) {//注意leftNum从1开始，hitArea从0开始
				hitArea[leftNum-1] = 1;
				hitArea[leftNum - 2>0?leftNum-2:0] = 1;
				hitArea[leftNum <DEAFAULT_GRID_X-1?leftNum:DEAFAULT_GRID_X-1] = 1;
			//	if (static_cast<int>(rotatedRect.center.x) % gridXWidth > gridXWidth / 2) {
			//		if(leftNum-1>0)hitArea[leftNum - 1]=1;
			//	}
			//	else{ if (leftNum + 1 < DEAFAULT_GRID_X)hitArea[leftNum + 1]=1;}//此段为根据中心确定附近栅格击中

			}
			else {for (; leftNum <= rightNum; leftNum++)hitArea[leftNum-1] = 1;}
			}
			}
		}
		
	
	
	Controler::getControler()->showImg(resultImg);//显示在sobel图上找到的可能的矩形
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