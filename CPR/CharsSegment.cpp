#include "stdafx.h"
#include "CharsSegment.h"
#include"util.h"
#include "Controler.h"

/*
//用于sort函数的comp函数，比较rect的中心坐标x值
bool CharsSegment::compX(const cv::Rect &rect1, const cv::Rect &rect2) {
	if (rect1.x < rect2.x) {
		return true;
	}
	else
	{
		return false;
	}
}*/

//用于sort函数的comp函数，比较rect的width
bool CharsSegment::compWidth(const cv::Rect &rect1, const cv::Rect &rect2) {
	if (rect1.width > rect2.width) {
		return true;
	}
	else
	{
		return false;
	}
}

void CharsSegment::genernalSegment(std::vector<CPlate> &vecCPlate) {
	for (int i = 0; i < vecCPlate.size(); i++) {
		cv::Mat plateImg = vecCPlate[i].plateImg.clone();
		cv::Mat plateBinaryImg;
		std::vector<cv::RotatedRect>charsRRect;	

		// cv::transform()
		segmentUsingFindRect(plateImg, plateBinaryImg, charsRRect);
	//	segmentUsingHybrid(plateImg, plateBinaryImg, charsRRect);
	//	segmentUsingProjection(plateImg, plateBinaryImg, charsRRect);
		vecCPlate[i].plateBinaryImg = plateBinaryImg;
		vecCPlate[i].characterRects = charsRRect;
	}
}

//判断矩形是否是字符区域的矩形
bool CharsSegment::vertifyCharacterSizes(const cv::Rect &Rect) {
	float ratio = Rect.width / (static_cast<float>(Rect.height));
	float area = Rect.area();
	if (( ratio < 1.1) && (area > 50 && area < 1000)&&Rect.height>20) {
		return true;
	}
	else
	{
		return false;
	}

}


/**
 * @brief 寻找矩形法分割字符
 * detail description
 * @param plateImg    原车牌图像
 * @param outputThresImg    输出的车牌二值图像，后期用于识别
 */
void  CharsSegment::segmentUsingFindRect(cv::Mat &plateImg, cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr) {
	
	
	
	cv::Mat plateImgThres = plateImg.clone();
	util::contrastEnhance(plateImgThres, plateImgThres, 10);


	cv::imshow("dddd", plateImgThres);
	cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "-"+"src"+"-" + std::to_string(std::rand())+".jpg",plateImgThres);
//	cv::waitKey(0);
	binaryInterface(plateImgThres, plateImgThres);
	
	bool istransform=false;
	//判断是否黄车牌，翻转灰度
	cv::Mat rectImg = cv::Mat(plateImgThres, cv::Rect(24, 0, 88, 28));
	if (cv::countNonZero(rectImg) > 1300) {//认为是黄车牌，二值化后字符为黑色

		cv::threshold(plateImgThres, plateImgThres, 128, 255, CV_THRESH_BINARY_INV);
		istransform = true;
	
	}
	outputThresImg = plateImgThres;
	util::clearUpDownBorder(plateImgThres, plateImgThres, 12);
//	outputThresImg = plateImgThres;


	cv::imshow("plateImgThres", plateImgThres);
	cv::imwrite(tempPath + std::to_string(Controler::getControler()->getCurrImgCount()) + "-" + "binary" + "-" + std::to_string(std::rand()) + ".jpg", plateImgThres);
//	cv::waitKey(0);

	//寻找字符矩形
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(plateImgThres, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator itBegin = contours.begin();
	std::vector<cv::Rect> vecRect;
	for (; itBegin != contours.end(); itBegin++) {
		cv::Rect2f rect;
	//	util::calcSafeRect(cv::minAreaRect(*itBegin), plateImgThres.size(), rect);
		rect=cv::boundingRect(*itBegin);
		if (vertifyCharacterSizes(rect)) {
			vecRect.push_back(rect);
		}
		else
		{

		}
	}


	if (vecRect.size() > 4) {//查找到多于四个的轮廓矩形，作为标准矩形进行中文字符查找
			
		std::sort(vecRect.begin(), vecRect.end(), SortWidth());
		cv::Rect stdRect = vecRect[2];
		if (stdRect.width < 8) {
			//	stdRect.width = width;
			stdRect.width = vecRect[1].width;
		}
		if (stdRect.width < 8) {
			//	stdRect.width = width;
			stdRect.width = vecRect[0].width;
		}
		if (stdRect.width < 8) {
			//	stdRect.width = width;
			stdRect.width = 12;
		}

		stdRect.width += 3;
		stdRect.height = 36;
		stdRect.x = 0;
		stdRect.y = 0;

		int step = 2;
		float confidence = cnnRecognizer->findChineseCharUsingRectMerge(plateImg,plateImgThres, stdRect,istransform);
	//	float confidence = cnnRecognizer->findChineseCharUsingSlide( plateImgThres, stdRect,step,istransform);
		if (confidence > 0.4&&vecRect.size() >4) {
		
			std::vector<cv::Rect> charRects;
			charRects.push_back(stdRect);
			for (int i = 0; i < vecRect.size(); i++) {
				if (vecRect[i].x + stdRect.width / 7 > stdRect.br().x) { 
					charRects.push_back(vecRect[i]);
				}
				else if (vecRect[i].x + stdRect.width /3 > stdRect.br().x)
				{
					cv::Rect interRect;
					interRect=(vecRect[i])&stdRect;
					if ((interRect.area() / float(vecRect[i].area())) < 0.4) { 
						charRects.push_back(vecRect[i]); 
					}
				}
			}
			if (charRects.size() > 4)
			{
				std::sort(charRects.begin(), charRects.end(), SortDesc());
				for (int i = 0; i < 7&&i<charRects.size(); i++) {
					charRects[i].y = 0;
					charRects[i].height = 37;
					if (charRects[i].width < stdRect.width-3) {
						charRects[i].x -= (stdRect.width - charRects[i].width+2) / 2;
					//	charRects[i].x -= 1;
						charRects[i].width = stdRect.width-2;
					//	charRects[i].width += 2;

					}
					else
					{
						charRects[i].x -= 1;
						charRects[i].width += 2;
					}

					cv::Point lb(charRects[i].tl().x, charRects[i].br().y);
					cv::RotatedRect rRect(charRects[i].tl(), lb, charRects[i].br());
					rr.push_back(rRect);
				}
			}
		}
		else
		{
			//后期用作查找不到的辅助,汉字识别度低于0.7
		}
	}
	else
	{
		return;
	}

	outputThresImg = plateImgThres;
	cv::imshow("plateImgThres", plateImgThres);
	//		cv::waitKey(0);
}


/**
 * @brief 
 * 结合寻找连通域矩形法和滑动窗口法分割字符，最高 53%的识别正确率，放弃
 * @param plateImg    原车牌图像
 * @param outputThresImg 输出的二值化图像，后期用于识别
 * @param rr 分割的字符矩形  
 */
void  CharsSegment::segmentUsingHybrid(cv::Mat &plateImg, cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr) {
	cv::Mat plateImgThres = plateImg.clone();
	cv::imshow("dddd", plateImgThres);
//	cv::waitKey(0);
	binaryInterface(plateImgThres, plateImgThres);
//	util::clearUpDownBorder(plateImgThres, plateImgThres, 8);
	//判断是否黄车牌，翻转灰度
	bool istranslform=false;
	cv::Mat rectImg = cv::Mat(plateImgThres, cv::Rect(24, 0, 88, 28));
	if (cv::countNonZero(rectImg) > 1300) {//认为是黄车牌，二值化后字符为黑色

		cv::threshold(plateImgThres, plateImgThres, 128, 255, CV_THRESH_BINARY_INV);
		istranslform = true;
		
	}
	util::clearUpDownBorder(plateImgThres, plateImgThres, 12);
	outputThresImg = plateImgThres;

	//寻找字符矩形
	std::vector<std::vector<cv::Point>>contours;
	cv::findContours(plateImgThres, contours, cv::RETR_EXTERNAL ,cv::CHAIN_APPROX_NONE);
	std::vector<std::vector<cv::Point>>::iterator itBegin = contours.begin();
	std::vector<cv::Rect> vecRect;

	for (; itBegin != contours.end(); itBegin++) {
		cv::Rect2f rect;
		util::calcSafeRect(cv::minAreaRect(*itBegin),plateImgThres.size(),rect);
		if (vertifyCharacterSizes(rect)) {
			vecRect.push_back(rect);
	//		cv::rectangle(showImg, rect, cv::Scalar(255), 2);
	//		cv::imshow("charsRect", showImg);
	//		cv::waitKey(0);
		}
		else
		{
		//	for (int i = 0; i < (*itBegin).size(); i++) {
		//		plateImgThres.at<uchar>((*itBegin)[i])=0;
		//	}
		}
	}
	if (vecRect.size()>6){//查找到多于六个的轮廓矩形，作为标准矩形进行中文字符查找
		std::sort(vecRect.begin(), vecRect.end(), SortWidth());
		cv::Rect stdRect = vecRect[2];
		if (stdRect.width > 25) {
			stdRect.width = vecRect[3].width;
		}
		if (stdRect.width < 8) {
			//	stdRect.width = width;
			stdRect.width = vecRect[1].width;
		}
		if (stdRect.width < 8) {
			//	stdRect.width = width;
			stdRect.width = vecRect[0].width;
		}
		if (stdRect.width < 8) {
			//	stdRect.width = width;
			stdRect.width = 12;
		}

		stdRect.width += 3;
		stdRect.height = 36;
		stdRect.x = 0;
		stdRect.y = 0;

		int step = 2;
		
		float confidence = cnnRecognizer->findChineseCharUsingSlide(plateImgThres, stdRect, 2,istranslform);
		if (confidence > 0.7&&vecRect.size() > 5) {

			std::vector<cv::Rect> charRects;
			charRects.push_back(stdRect);
			for (int i = 0; i < vecRect.size(); i++) {
				if (vecRect[i].x + stdRect.width / 6 > stdRect.br().x) {
					charRects.push_back(vecRect[i]);
				}
				else if (vecRect[i].x + stdRect.width / 2 > stdRect.br().x)
				{
					cv::Rect interRect;
					interRect = (vecRect[i])&stdRect;
					if ((interRect.area() / vecRect[i].area()) < 0.5)charRects.push_back(vecRect[i]);
				}
			}
			if (charRects.size() > 4)
			{
				std::sort(charRects.begin(), charRects.end(), SortDesc());
				for (int i = 0; i < 7 && i < charRects.size(); i++) {
					charRects[i].y = 0;
					charRects[i].height = 36;
					if (charRects[i].width < stdRect.width - 3) {
						charRects[i].x -= (stdRect.width - charRects[i].width - 2) / 2;
						//	charRects[i].x -= 1;
						charRects[i].width = stdRect.width - 2;
						//	charRects[i].width += 2;

					}
					else
					{
						charRects[i].x -= 1;
						charRects[i].width += 2;
					}

					cv::Point lb(charRects[i].tl().x, charRects[i].br().y);
					cv::RotatedRect rRect(charRects[i].tl(), lb, charRects[i].br());
					rr.push_back(rRect);
				}
			}
		}
		else
		{
		
			int height = plateImgThres.rows;
				cv::Point bl = stdRect.tl();
				bl.y = bl.y + stdRect.height;
				cv::RotatedRect rRect(stdRect.tl(), bl, stdRect.br());
				rr.push_back(rRect);
				int  startPos = stdRect.br().x;
				int  endPos = startPos + stdRect.width;
				int i = 0;
				while (i < 6) {

					if (endPos < plateImgThres.cols + 5) {

						cnnRecognizer->findBestPos(plateImgThres, startPos, endPos, 1);
						cv::RotatedRect rRect(cv::Point(startPos, 0), cv::Point(startPos, height), cv::Point(endPos, height));
						rr.push_back(rRect);



						if (i == 0) {
							startPos = endPos + stdRect.width / 5.0;
							endPos = startPos + stdRect.width;
						}
						else
						{
							startPos = endPos + stdRect.width / 8.0;
							endPos = startPos + stdRect.width;
						}
					}

					i++;
				}
			
		}			
				
	}  
	else if(vecRect.size()>2)
	{
		
	std::sort(vecRect.begin(), vecRect.end(), SortWidth());
	cv::Rect stdRect = vecRect[2];
	if (stdRect.width < 8) {
		//	stdRect.width = width;
		stdRect.width = vecRect[1].width;
	}
	if (stdRect.width < 8) {
		//	stdRect.width = width;
		stdRect.width = vecRect[0].width;
	}
	if (stdRect.width < 8) {
		//	stdRect.width = width;
		stdRect.width = 12;
	}

	stdRect.width += 3;
	stdRect.height = 36;
	stdRect.x = 0;
	stdRect.y = 0;

	int step = 2;
	float confidence = cnnRecognizer->findChineseCharUsingSlide(plateImgThres, stdRect, 2,istranslform);

	//后期用作查找不到的辅助,汉字识别度低于0.7,矩形框数目小于4
	int height = plateImgThres.rows;
	if (confidence > 0.7) {
		cv::Point bl = stdRect.tl();
		bl.y = bl.y + stdRect.height;
		cv::RotatedRect rRect(stdRect.tl(), bl, stdRect.br());
		rr.push_back(rRect);
		int  startPos = stdRect.br().x;
		int  endPos = startPos + stdRect.width;
		int i = 0;
		while (i < 6) {

			if (endPos < plateImgThres.cols + 5) {

				cnnRecognizer->findBestPos(plateImgThres, startPos, endPos, 1);
				cv::RotatedRect rRect(cv::Point(startPos, 0), cv::Point(startPos, height), cv::Point(endPos, height));
				rr.push_back(rRect);
				if (i == 0) {
					startPos = endPos + stdRect.width / 5.0;
					endPos = startPos + stdRect.width;
				}
				else
				{
					startPos = endPos + stdRect.width / 8.0;
					endPos = startPos + stdRect.width;
				}
			}

			i++;
		}
	}
	}
	
    	outputThresImg = plateImgThres;
		cv::imshow("plateImgThres", plateImgThres);
//		cv::waitKey(0);
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
	binaryInterface(plateImgThres, plateImgThres);
		
	//clear edge
		//判断是否黄车牌，翻转灰度
	cv::Mat rectImg = cv::Mat(plateImgThres, cv::Rect(24, 0, 88, 24));
	if (cv::countNonZero(rectImg) > 1450) {//认为是黄车牌，二值化后字符为黑色

		cv::threshold(plateImgThres, plateImgThres, 128, 255, CV_THRESH_BINARY_INV);
	}
	util::clearUpDownBorder(plateImgThres, plateImgThres, 12);
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
void CharsSegment::binaryInterface(cv::Mat &inputImg, cv::Mat &outputImg) {
	outputImg = inputImg.clone(); 
	cv::Mat wallnerImg = outputImg.clone();
	if (inputImg.channels() == 3) {
	//	cv::cvtColor(inputImg, inputImg, cv::COLOR_BGR2GRAY);
		
		cv::cvtColor(wallnerImg, wallnerImg, cv::COLOR_BGR2GRAY);

		
		cv::Ptr<cv::CLAHE> cle = cv::createCLAHE();
		cle->setClipLimit(4.);    // (int)(4.*(8*8)/256)
		cle->setTilesGridSize(cv::Size(8, 8)); // 将图像分为8*8块

		std::vector<cv::Mat> BGR;
		cv::cvtColor(outputImg, outputImg, cv::COLOR_BGR2YCrCb);
		cv::split(outputImg, BGR);
		cle->apply(BGR[0], BGR[0]);
	//	util::ACE(BGR[0]);
	//	cle->apply(BGR[1], BGR[1]);
	//	cle->apply(BGR[2], BGR[2]);
		merge(BGR, outputImg);  
		cv::cvtColor(outputImg, outputImg, cv::COLOR_YCrCb2BGR);  
		cv::imshow("CLAHE Colorful Img", outputImg);
//		cv::waitKey(0);
		cv::cvtColor(outputImg, outputImg, cv::COLOR_BGR2GRAY);
	}


	
	
	
//	spatialOstu(outputImg, 0, 2, 4);
	util::spatialOstu(outputImg, 2, 2);
//	util::Wallner(wallnerImg, wallnerImg);
//	util::Wallner(outputImg, outputImg);//基于积分图的快速局部平均阈值算法，S取一定值，边缘较细，效果比局部ostu好，采用
	//cv::adaptiveThreshold(adatativetempImg, adatativetempImg, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, -5);//常数C难以适应全部，淘汰
	//cv::threshold(affinedGray, affinedGray, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);//对光照不均不友好，淘汰
	cv::imshow("spatialOSTU", outputImg);
	cv::imshow("wallner", wallnerImg);
//	cv::waitKey(0);
}


CharsSegment::CharsSegment()
{
	std::string caffeModel = "D:/VS/CPR/model/CharacterRecognization.caffemodel";
	std::string prototext = "D:/VS/CPR/model/CharacterRecognization.prototxt";
	cnnRecognizer = new CNNRecognizer(prototext, caffeModel);

}


CharsSegment::~CharsSegment()
{
}
