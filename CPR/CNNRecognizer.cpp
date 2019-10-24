#include "stdafx.h"
#include "CNNRecognizer.h"
#include "PlateLocate.h"
#include "CPlate.h"
CNNRecognizer::CNNRecognizer(std::string &prototxt,std::string &caffemodel)
{
	net = cv::dnn::readNetFromCaffe(prototxt, caffemodel);//构造函数中读取模型
}

//从给定的起始位置和结束位置，在一定范围内寻找字符可信度最大的位置，并返回可信度，及其起始位置和结束位置
float CNNRecognizer::findBestPos(const cv::Mat &plateImg, int &startPos,int &endPos, uchar step) {
	int height = plateImg.rows;
	int proposalStartPos;
	int proposalEndPos;
	float maxConfidence=0;
	int tempStart = startPos;
	int tempEnd = endPos;
	std::map<uchar, hitCount> mapPos;

	for (int i = 0; i < 3; i++) {
		tempStart++;
		for (int j = 0; j < 3; j++) {
			tempEnd++;
			if (tempEnd > plateImg.cols)tempEnd = plateImg.cols;
			cv::Rect rect(cv::Point(tempStart, 0), cv::Point(tempEnd, height));
			cv::Mat proposalImg = plateImg(rect).clone();

			if (proposalImg.channels() == 3)
				cv::cvtColor(proposalImg, proposalImg, cv::COLOR_BGR2GRAY);
			




			if (1) {
				std::vector<std::vector<cv::Point>> contours;
				cv::findContours(proposalImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
				cv::Rect maxRect;
				int max=0;
				maxRect.height = 0;
				maxRect.width = 0;

				if (contours.size() > 1) {
					for (int j = 0; j < contours.size(); j++) {
						cv::Rect rect = cv::boundingRect(contours[j]);
						if (rect.area() > maxRect.area()) {

							maxRect = rect;
							std::vector<std::vector<cv::Point>> fillcontours;
							fillcontours.push_back(contours[max]);
							if(max!=0)cv::fillPoly(proposalImg, fillcontours, cv::Scalar(0));
							max = j;
						}
						else
						{
							std::vector<std::vector<cv::Point>> fillcontours;
							fillcontours.push_back(contours[j]);
							cv::fillPoly(proposalImg, fillcontours, cv::Scalar(0));
						}
					}

				}
			}


			cv::copyMakeBorder(proposalImg, proposalImg, 1,1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
			
			cv::Mat inputBlob = cv::dnn::blobFromImage(proposalImg, 1 / 255.0,
				cv::Size(CHAR_INPUT_W, CHAR_INPUT_H), cv::Scalar(), false);
			net.setInput(inputBlob, "data");
			cv::Mat  probMat = net.forward();
			float *prob = (float *)probMat.data;
			float confidence = *std::max_element(prob+31, prob + 65);
			int code = std::max_element(prob + 31, prob + 65) - prob - 31;
			
			//储存可能性最大的起止位置
			if (confidence > maxConfidence) {
				qDebug() << i << ":" << j<<"-"<< "max Confidence:" << confidence;
				maxConfidence = confidence;
				proposalStartPos = tempStart;
				proposalEndPos = tempEnd;
			}
			qDebug() << i << ":" << j<<"-" << "chars Confidence:" << confidence;
			cv::imshow("chars", proposalImg);
	//		cv::waitKey(0);

			//如果可能性大于0.9，则该识别结果计数+1，后期根据计数结果寻找，后面没有用到，因为不实用，容易出错
			if (confidence > 0.9) {
				mapPos[code].count++;
				mapPos[code].totalConfidence+=confidence;
				if (confidence > mapPos[code].maxConfidence) {
					std::vector<int> pos;
					pos.push_back(tempStart);
					pos.push_back(tempEnd);
					mapPos[code].vecPos = pos;
					mapPos[code].maxConfidence = confidence;
				}
				
			}
		}
		tempEnd = endPos;
		
	}
	startPos = proposalStartPos;
	endPos = proposalEndPos;
	uchar mapSize = mapPos.size();

	/*
	if (mapSize >= 2) {
		std::vector<int >vecPos;
		float maxConfidence=0;
		std::map<uchar,hitCount>::iterator itBegin = mapPos.begin();
		for (; itBegin != mapPos.end(); itBegin++) {
			if (itBegin->second.totalConfidence/ itBegin->second.count> maxConfidence) {
				maxConfidence = itBegin->second.totalConfidence/itBegin->second.count;
				vecPos = itBegin->second.vecPos;
			}
			
		}
		startPos = vecPos[0];
		endPos = vecPos[1];
	}*/
	return maxConfidence;
}



float CNNRecognizer::findChineseCharUsingRectMerge(const cv::Mat &plateImg, cv::Mat plateBinaryImg, cv::Rect &stdRect, bool istransform) {
	
	cv::Mat plateBinary= plateBinaryImg.clone();
	cv::Mat srcplateImg = plateImg.clone();
	
	double globalSigma;

	double globalThreshVal = util::getThreshVal_Otsu_8u(plateBinary, globalSigma);
	int width = plateBinary.cols /2;
	int height = plateBinary.rows / 2;
	std::vector<double> local_globalSigma;

	// iterate through grid

	double sigma;
	cv::Mat src_cellLine = cv::Mat(plateBinary, cv::Range(14, 24), cv::Range(10, 60));
	double cellThreshVal = util::getThreshVal_Otsu_8u(src_cellLine, sigma);//ostu阈值法


	src_cellLine = cv::Mat(plateBinary, cv::Range(14, 24), cv::Range(60, 125));
	double cellThreshVal2 = util::getThreshVal_Otsu_8u(src_cellLine, sigma);//ostu阈值法


	for (int j = 0; j < 2; j++) {

		cv::Mat src_cellLine = cv::Mat(plateBinary, cv::Range(18, 20), cv::Range(30, 100));
		//	double cellThreshVal = util::getThreshVal_Otsu_8u(src_cellLine, sigma);
		for (int i = 0; i < 2; i++) {
			cv::Mat src_cell = cv::Mat(plateBinary, cv::Range(j * height, (j + 1) * height), cv::Range(i * width, (i + 1) * width));
			cv::imshow("src_cell", src_cell);
			if (i > 0) {
				cv::threshold(src_cell, src_cell, cellThreshVal2, 255, CV_THRESH_BINARY);
			}
			else
			{
				cv::threshold(src_cell, src_cell, cellThreshVal, 255, CV_THRESH_BINARY);
			}

		}
	}

	cv::imshow("plateBinary", plateBinary);
//	cv::waitKey();  */
	//将白点数小于3的列置0
	for (int i = 0; i < plateBinary.cols / 3; i++) {
		int whiteCount = 0;
		for (int l = 0; l < plateBinary.rows; l++) {
			if (plateBinary.at<uchar>(l, i) > 0) whiteCount++;
		}
		if (whiteCount < 3) {

			for (int l = 0; l < plateBinary.rows; l++) {
				plateBinary.at<uchar>(l, i) = 0;
			}
		}
	}

	cv::imshow("plateBinary", plateBinary);
//	cv::waitKey();  
	//扫描行列得到汉字候选矩形，后期应进一步筛选融合
	std::vector<cv::Rect> proposalRect;
	bool ChangeStartPos=false;
	int nextStartPos = 0;
	int inEmptyCol = -1;
	int length = 0;
	int startPos = 0;
	int endPos = 0;
 	for (int i = 0; i < plateBinary.cols / 3; i++) {
		
			int whiteCount=0;			
					
			for (int l = 0; l < plateBinary.rows; l++) {
				if (plateBinary.at<uchar>(l, i) > 0) whiteCount++;
			//	if (plateBinary.at<uchar>(l, i + 1) > 0)whiteCount++;
			}

			//为字符区域
			if (whiteCount > 5) {
				if (inEmptyCol == -1) { 
					startPos = i;
					inEmptyCol = 0;
				}
				length++;				
			}

			//不为字符区域
			if (whiteCount <=5&&(length>stdRect.width*0.7)) {
				
				if (inEmptyCol == 0)endPos = i;
				inEmptyCol = 1;
			}

			//空行但可能为字符区域,继续计数
			if (whiteCount <=5 && length<stdRect.width*0.7&&inEmptyCol!=-1) {
			
				length++;
				if (!ChangeStartPos) {
					ChangeStartPos = true;
					nextStartPos = i;
				}
				
			}
			
			if (inEmptyCol == 1) {
				if (length > 1.1*stdRect.width) {
					proposalRect.push_back(cv::Rect(endPos-stdRect.width, 0, stdRect.width, 36));
					proposalRect.push_back(cv::Rect(startPos, 0, stdRect.width, 36));
				}
				else //if(length)
				{
					if ((startPos - 1) > 0) {
						proposalRect.push_back(cv::Rect((startPos - 1), 0, length+1, 36));
					}
					else
					{
						proposalRect.push_back(cv::Rect((startPos), 0, length, 36));
					}
					
				}	

				if (ChangeStartPos) {
					ChangeStartPos = false;
					length = 0;
					startPos = 0;
					i= nextStartPos;
					nextStartPos = 0;
					endPos = 0;
					inEmptyCol = -1;
				}
				else
				{
					length = 0;
					startPos =0;
					endPos = 0;
					inEmptyCol = -1;
				}
				
				
	
			}
	
	}


	//对proposalRects进行中文字符识别，得到confidence最大的一个；
	float maxConfidence = 0;
	int i = 0;
	cv::Rect bestRect;
	cv::Mat maxConfidenceImg;
	for (auto charRect : proposalRect) {
		cv::Mat proposalImg = plateImg(charRect);
		if (proposalImg.channels() == 3)
			cv::cvtColor(proposalImg, proposalImg, cv::COLOR_BGR2GRAY);
		util::contrastEnhance(proposalImg, proposalImg, 10);

	
		if (istransform) {
			cv::threshold(proposalImg, proposalImg, 128, 255, cv::THRESH_OTSU + cv::THRESH_BINARY_INV);
		}
	
		
		
		//基于双峰的阈值化
		/*
		int threshold = util::GetMinimumThreshold(proposalImg);
		double sigma;
		if (threshold == -1)threshold = util::getThreshVal_Otsu_8u(proposalImg, sigma);
		threshold = threshold * 1.1;
		if (istransform) {
			cv::threshold(proposalImg, proposalImg, threshold, 255, cv::THRESH_BINARY_INV);
		}
		else
		{
			cv::threshold(proposalImg, proposalImg, threshold, 255, cv::THRESH_BINARY );
		}*/

	//	cv::threshold(proposalImg, proposalImg, 128, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);
	//	float percent = cv::countNonZero(proposalImg) / (proposalImg.rows*proposalImg.cols);

		cv::copyMakeBorder(proposalImg, proposalImg, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
	//	cv::copyMakeBorder(proposalImg, proposalImg, 1, 1, 2, 2, cv::BORDER_REPLICATE);
		cv::Mat inputBlob = cv::dnn::blobFromImage(proposalImg, 1 / 255.0,
			cv::Size(CHAR_INPUT_W, CHAR_INPUT_H), cv::Scalar(), false);
		net.setInput(inputBlob, "data");
		cv::Mat probMat = net.forward();
		float *prob = (float *)probMat.data;
		int code = std::max_element(prob, prob + 31) - prob;
		float confidence = *std::max_element(prob, prob + 31);
		if (confidence >= maxConfidence) {
			qDebug() << i << "-"<<chars_code[code].c_str() << "max chinese confidence: " << confidence;
			maxConfidence = confidence;
			bestRect = charRect;
		}

		qDebug() << i << "-" << chars_code[code].c_str() << "chinese confidence:" << confidence;
		cv::imshow("chinese", proposalImg);
	//	cv::waitKey(0);
		i++;
	}
	stdRect = bestRect;
	return maxConfidence;
}


//在车牌中查找中文字符的可能位置，返回可能值最大的那个
float CNNRecognizer::findChineseCharUsingSlide(const cv::Mat &plateImg,   cv::Rect &stdRect, uchar step, bool istransform) {
	
	float maxConfidence=0;
	//std::map<uchar, hitCount> mapPos;
	cv::Rect proposalRect;
	int i = 0;
	for (; stdRect.x < plateImg.cols / 4; stdRect.x += step){
		cv::Mat proposalImg = plateImg(stdRect);
		if (proposalImg.channels() == 3)
			cv::cvtColor(proposalImg, proposalImg, cv::COLOR_BGR2GRAY);

		util::contrastEnhance(proposalImg, proposalImg, 10);


		if (istransform) {
			cv::threshold(proposalImg, proposalImg, 128, 255, cv::THRESH_OTSU + cv::THRESH_BINARY_INV);
		}

		cv::copyMakeBorder(proposalImg, proposalImg, 1, 1, 2, 2, cv::BORDER_CONSTANT, cv::Scalar(0));
		cv::Mat inputBlob = cv::dnn::blobFromImage(proposalImg, 1 / 255.0,
			cv::Size(CHAR_INPUT_W, CHAR_INPUT_H), cv::Scalar(), false);
		net.setInput(inputBlob, "data");
		cv::Mat probMat = net.forward();
		float *prob = (float *)probMat.data;
		int code= std::max_element(prob, prob + 31)-prob;
		float confidence = *std::max_element(prob, prob + 31);
		if (confidence > maxConfidence) {
			qDebug() << i << "-" << "max chinese confidence: " << confidence;
			maxConfidence = confidence;
			proposalRect = stdRect;
		}
		
		qDebug() << i << "-" <<"chinese confidence:"<< confidence;
		cv::imshow("chinese", proposalImg);
//		cv::waitKey(0);
		i++;		
	}
	stdRect = proposalRect;
	
	return maxConfidence;
}


std::string CNNRecognizer::recognizeCharacter(cv::Mat &characterImg,CharType characterType, double &totalconfidence) {
	if (characterImg.channels() == 3) { cv::cvtColor(characterImg, characterImg, CV_BGR2GRAY); }
	cv::Mat inputBlob = cv::dnn::blobFromImage(characterImg, 1 / 255.0, cv::Size(CHAR_INPUT_W, CHAR_INPUT_H), cv::Scalar(),false);
	net.setInput(inputBlob,"data");
	cv::Mat probMat=net.forward();
	std::string decode;
	decode = decodePlateNormal(chars_code, probMat, characterType,totalconfidence);
	return decode;
}


std::string CNNRecognizer::decodePlateNormal(std::vector<std::string> mappingTable,cv::Mat probMat,CharType characterType, double &totalconfidence) {
	std::string decode;
	float confidence=0;
	float *prob = (float *)probMat.data;
	for (int i = 0; i < 65; i++) {
		//qDebug() << "i=" << i << ":" << prob[i];
	}
	
 	if (characterType == CHINESE) {
		decode += mappingTable[std::max_element(prob, prob + 31) - prob];
		confidence += *std::max_element(prob, prob + 31);
		totalconfidence+= *std::max_element(prob, prob + 31);
		qDebug() << "confidence" << confidence;
	}
	if (characterType == LETTER) {
		decode += mappingTable[std::max_element(prob + 41, prob + 65) - prob];
		confidence += *std::max_element(prob+41, prob + 65);
		totalconfidence += *std::max_element(prob + 41, prob + 65);
		qDebug() << "confidence" << confidence;
	}
	if (characterType == LETTER_NUM) {
		decode += mappingTable[std::max_element(prob + 31, prob + 65) - prob];
		confidence += *std::max_element(prob + 31, prob + 65);
		totalconfidence += *std::max_element(prob + 31, prob + 65);
		qDebug() << "confidence" << confidence;
	}
	return decode;
}
CNNRecognizer::~CNNRecognizer()
{
}
