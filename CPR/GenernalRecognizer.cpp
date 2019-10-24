#include "stdafx.h"
#include "GenernalRecognizer.h"
#include "PlateLocate.h"
#include"CPlate.h"
#include"util.h"

GenernalRecognizer::GenernalRecognizer()
{
}


GenernalRecognizer::~GenernalRecognizer()
{
}
void GenernalRecognizer::SegmentBasedSequenceRecognition(std::vector<CPlate> &vecPlate) {
	
	
	
	
	for (int i = 0; i < vecPlate.size(); i++) {
		CPlate &plate = vecPlate[i];
		plate.srcImg.release();

		std::string characters;
		cv::Mat	affinedImg;
		cv::Mat tempImg2;
		tempImg2 = plate.plateImg.clone();
		affinedImg = plate.plateImg.clone();
		cv::Mat showImg = tempImg2.clone();
//		affinedImg = plate.plateImg.clone();
	//	if (affinedImg.channels() == 3) {
	//		cv::cvtColor(affinedImg, affinedImg, CV_BGR2GRAY);
	//	}
	

		bool istransform=false;
		if (tempImg2.channels() == 3) {

			cv::Ptr<cv::CLAHE> cle = cv::createCLAHE();
			cle->setClipLimit(4.);    // (int)(4.*(8*8)/256)
			cle->setTilesGridSize(cv::Size(8, 8)); // 将图像分为8*8块

			std::vector<cv::Mat> BGR;
			cv::cvtColor(tempImg2, tempImg2, cv::COLOR_BGR2YCrCb);
			cv::split(tempImg2, BGR);
			cle->apply(BGR[0], BGR[0]);
			//	util::ACE(BGR[0]);
			merge(BGR, tempImg2);
			cv::cvtColor(tempImg2, tempImg2, cv::COLOR_YCrCb2BGR);
			cv::imshow("CLAHE Colorful Img", tempImg2);
			//	cv::waitKey(0);
			cv::cvtColor(tempImg2, tempImg2, cv::COLOR_BGR2GRAY);
		}
		util::spatialOstu(tempImg2, 2, 2);
		//判断是否黄车牌，翻转灰度
		cv::Mat rectImg = cv::Mat(tempImg2, cv::Rect(24, 0, 88, 28));
		if (cv::countNonZero(rectImg) > 1300) {//认为是黄车牌，二值化后字符为黑色

			cv::threshold(tempImg2, tempImg2, 128, 255, CV_THRESH_BINARY_INV);
			istransform = true;
		}
		util::clearUpDownBorder(tempImg2, tempImg2, 12);
		



		for (int i = 0; i < plate.characterRects.size(); i++) {
			cv::RotatedRect rRect = plate.characterRects[i];
			cv::Rect2f characterRect;
			util::calcSafeRect(rRect, affinedImg.size(), characterRect);
			if (characterRect.width == 0)continue;
			cv::rectangle(showImg, characterRect, cv::Scalar(128));

			cv::imshow("charRect", showImg);

			cv::Mat tempImg = cv::Mat(affinedImg, characterRect).clone();
			cv::Mat garyImg = tempImg.clone();
		//	cv::threshold(tempImg, tempImg, 128, 255, cv::THRESH_OTSU + cv::THRESH_BINARY);
			
			if (tempImg.channels() == 3)cv::cvtColor(tempImg, tempImg, cv::COLOR_BGR2GRAY);

			if(i!=0){
			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(tempImg, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
			cv::Rect maxRect;
			int max=-1;
			maxRect.height = 0;
			maxRect.width = 0;


			if (contours.size() > 1) {
				for (int j = 0; j < contours.size(); j++) {
					cv::Rect rect = cv::boundingRect(contours[j]);
					if (rect.area() > maxRect.area()) {
						
						maxRect = rect;
						
						if (max !=-1) {
							
						//	cv::drawContours(tempImg, contours, max, cv::Scalar(0), -1);
						}
						max = j;
					}
					else
					{

				//		cv::drawContours(tempImg, contours, j, cv::Scalar(0), -1);
					}
				}
					
			}
			}

			int threshold=util::GetMinimumThreshold(tempImg);
			double sigma;
			if (threshold == -1)threshold = util::getThreshVal_Otsu_8u(tempImg,sigma);
			threshold = threshold * 1.1;
			if (istransform) {
				cv::threshold(tempImg, tempImg, threshold, 255, cv::THRESH_BINARY_INV );
			}
			{
				cv::threshold(tempImg, tempImg, threshold, 255, cv::THRESH_BINARY );
			}

			util::contrastEnhance(garyImg, tempImg, 10);

			if (istransform) {
				cv::threshold(tempImg, tempImg, 128, 255, cv::THRESH_OTSU + cv::THRESH_BINARY_INV);
			}

			/*
			if (istransform) {
				cv::threshold(tempImg, tempImg, 255, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);
			}
			else
			{
				cv::threshold(tempImg, tempImg, 255, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
			}*/			
			cv::copyMakeBorder(tempImg, tempImg, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));

			cv::imshow("semgIMG", tempImg);
		//	cv::waitKey(0);
			if (i == 0) {
				
			//	tempImg = cv::Mat(plate.plateImg, characterRect).clone();
			//	cv::copyMakeBorder(tempImg, tempImg, 1, 1, 1, 1, cv::BORDER_CONSTANT, cv::Scalar(0));
				characters += recognizeCharacter(tempImg, CHINESE,plate.confidence);
			//	cv::imshow("semgIMG", tempImg);
			//			cv::waitKey(0);
			}
			else if (i == 1) {
				characters += recognizeCharacter(tempImg, LETTER, plate.confidence);
			}
			else if (i == 2) {
				characters += recognizeCharacter(tempImg, LETTER_NUM, plate.confidence);
			}
			else if (i == 7) {

			}
			else
			{
				characters += recognizeCharacter(tempImg, LETTER_NUM, plate.confidence);
			}

			//character = util::GBKToUTF8(character.data());
			qDebug() << QString::fromStdString(characters);
		}
		plate.setPlateChars(characters);


	}

	
	//去除重复车牌中的低confidence的车牌

	std::vector<CPlate>::iterator itBegin = vecPlate.begin();
	
	
	
	while (itBegin != vecPlate.end()) {
		
		//去除分数较低的车牌，但车牌检出大幅减少。
		/*
		if (itBegin->confidence<4.0)
		{
			itBegin = vecPlate.erase(itBegin);
			continue;
		}*/
		std::vector<CPlate>::iterator secCPlate= itBegin + 1;
		bool isErase = false;
		while (secCPlate!=vecPlate.end())
		{
			
			if (itBegin->plateRotaRect.center.x - secCPlate->plateRotaRect.center.x < secCPlate->plateRotaRect.size.width
				&&itBegin->plateRotaRect.center.y - secCPlate->plateRotaRect.center.y < secCPlate->plateRotaRect.size.height) {
				if (itBegin->confidence > secCPlate->confidence) {
					
					secCPlate =vecPlate.erase(secCPlate);
				}
				else
				{
				//	itBegin = vecPlate.erase(itBegin);
					isErase = true;
					break;				
				}
			}
			else {
				secCPlate++;
			}

		}
		if (isErase) {
			itBegin = vecPlate.erase(itBegin);
		}
		else
		{
			itBegin++;
		}
		
	}

	// vecPlate.swap(tempVec);
	
}