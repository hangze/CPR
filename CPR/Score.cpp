#include "stdafx.h"
#pragma once
#include"Controler.h"
#include "Score.h"
#include"util.h"

Score::Score()
{
}


Score::~Score()
{
}


int Score::recordCPlate(std::vector<CPlate>&currPlates,std::string imgPath) {
	std::string imageName = imgPath.substr(imgPath.find_last_of("/") + 1, 9);
	XMLNode xNode, rectangleNodes;
	xNode = xMainNode.addChild("image");
	xNode.addChild("imageName").addText(imageName.c_str());
	rectangleNodes = xNode.addChild("taggedRectangles");

	for (int i = 0; i < currPlates.size(); i++) {
		XMLNode rectangleNode = rectangleNodes.addChild("taggedRectangle");
		CPlate currPlate = currPlates[i];

		cv::RotatedRect rotatedRect = currPlate.getRotatedRect();
		rectangleNode.addAttribute("x=", std::to_string(int(rotatedRect.center.x)).c_str());
		rectangleNode.addAttribute("y=", std::to_string(int(rotatedRect.center.y)).c_str());
		rectangleNode.addAttribute("width=", std::to_string(int(rotatedRect.size.width)).c_str());
		rectangleNode.addAttribute("height=", std::to_string(int(rotatedRect.size.height)).c_str());
		rectangleNode.addAttribute("rotattion=", std::to_string(int(rotatedRect.angle)).c_str());
		std::string platechars = currPlate.getPlateChars();
		if (platechars.empty()) {
			platechars = u8"未A00000";
		}
		rectangleNode.addText(platechars.c_str());
	}
	recoCPlateMap[imageName] = currPlates;
	return 0;
}

int Score::calcuAccuracy(std::string imgPath,int allTime) {
	
	std::string directory = imgPath.substr(0, imgPath.find_last_of("/"));
	//用xml文件保存识别结果	
	std::string resultPath = directory + "/Result.xml";
	xMainNode.writeToFile(resultPath.c_str());
	//xMainNode.deleteClear();

	//compare with GroundTruth File
		XMLNode::setGlobalOptions(XMLNode::char_encoding_UTF8);
		std::string GroundTruthPath = directory + "/GroundTruth_others.xml";
		std::map<std::string, std::vector<CPlate>> GTCPlateMap;
		getGroundTruth(GroundTruthPath, GTCPlateMap);
		//recoCPlateMap.clear();
		//GroundTruthPath = directory + "/Result.xml";
		//getGroundTruth(GroundTruthPath, recoCPlateMap);


		int detectPlateCount = 0;//定位出的车牌总数
		int noDetectPlateCount = 0;
		int locateCorrectPlateCount = 0;//定位正确的车牌总数			
		std::vector<float>icdar2003_precise_all;//准确率，即全部识别车牌的最佳匹配度的积分/全部识别车牌数
		float pricise = 0;//定位准确率


		int recoPlateCount = 0;//识别出字符的车牌总数
		int zeroErrorRecoPlateCount = 0;//零字符差错的车牌总数
		int oneErrorRecoPlateCount = 0;//1字符差错的车牌总数
		int errorRecoPlateCount = 0;
		int gtPlateCount = 0;//标准车牌总数
		int chineseCorrectCount = 0;//中文正确数
		float recall = 0;
		std::vector<float>icdar2003_recall_all;//查全率，即全部标准车牌的最佳匹配度的积分/全部标准车牌数

											   //计算定位准确率及字符准确率  每一张标准车牌的匹配度的积分/标准车牌数			
		std::map<std::string, std::vector<CPlate>>::iterator mapIter = recoCPlateMap.begin();
		for (; mapIter != recoCPlateMap.end(); mapIter++) {
			std::string imgName = mapIter->first;
			std::vector<CPlate> vecRecoCPlate = mapIter->second;
			//std::vector<CPlate> gtVecCPlate = GTCPlateMap[imgName];
			std::map<std::string, std::vector<CPlate>>::iterator it = GTCPlateMap.find(imgName);
			std::vector<CPlate> gtVecCPlate;
			if (it != GTCPlateMap.end()) {
				gtVecCPlate = it->second;
			}


			for (auto recoPlate : vecRecoCPlate) {
				detectPlateCount++;
				std::string recoChars = recoPlate.getPlateChars();
				cv::RotatedRect recoRRect = recoPlate.plateRotaRect;
				cv::Rect_<float> recoRect;
				util::calcSafeRect(recoRRect, recoPlate.srcSize, recoRect);
				float bestMatch = 0.f;
				CPlate matchCPlate;

				//std::string gtChars;
				for (auto gtPlate : gtVecCPlate) {
					//std::string gtChars = gtPlate.getPlateChars();
					cv::RotatedRect gtRRect = gtPlate.plateRotaRect;
					cv::Rect_<float> gtRect;
					util::calcSafeRect(gtRRect, gtPlate.srcSize, gtRect);
					gtRect = gtPlate.plateRotaRect.boundingRect();
					//计算最佳匹配度

					cv::Rect interRRect = recoRect & gtRect;
					float match = float(2 * interRRect.area()) / (recoRect.area() + gtRect.area());
					if ((match - bestMatch) > 0.1f) {
						bestMatch = match;
						matchCPlate = gtPlate;
					}
				}
				icdar2003_precise_all.push_back(bestMatch);

			}


		}
		if (icdar2003_precise_all.size() > 0) {
			pricise = std::accumulate(icdar2003_precise_all.begin(),
				icdar2003_precise_all.end(), 0.0) / icdar2003_precise_all.size();
		}


		//计算查全率 每一张识别车牌的最佳匹配度的积分/识别车牌数
		std::map < std::string, std::vector<CPlate>>::iterator gtIter = GTCPlateMap.begin();
		for (; gtIter != GTCPlateMap.end(); gtIter++) {
			std::string gtImgName = gtIter->first;
			std::vector<CPlate> gtVecCPlate = gtIter->second;
			std::map<std::string, std::vector<CPlate>>::iterator it = recoCPlateMap.find(gtImgName);
			std::vector<CPlate> recoVecCPlate;
			if (it != recoCPlateMap.end()) {
				recoVecCPlate = it->second;
			}


			for (auto gtCPlate : gtVecCPlate) {
				gtPlateCount++;
				std::string gtImgName = gtCPlate.getPlateChars();
				//gtImgName = gtImgName.substr(gtImgName.find(":"));
				cv::RotatedRect gtRRect = gtCPlate.plateRotaRect;
				cv::Rect_<float> gtRect;
				util::calcSafeRect(gtRRect, gtCPlate.srcSize, gtRect);
				gtRect = gtCPlate.plateRotaRect.boundingRect();

				std::string recoImgName;
				float bestMatch = 0.;
				CPlate matchCPlate;
				for (auto recoCPlate : recoVecCPlate) {

					cv::RotatedRect recoRRect = recoCPlate.plateRotaRect;
					cv::Rect_<float> recoRect;
					util::calcSafeRect(recoRRect, recoCPlate.srcSize, recoRect);
					cv::Rect_<float> interRect = gtRect & recoRect;
					float match = float(2 * interRect.area()) / (gtRect.area() + recoRect.area());
					if ((match - bestMatch) > 0.1f) {
						bestMatch = match;
						matchCPlate = recoCPlate;
						recoImgName = recoCPlate.getPlateChars();
					}
				}

				icdar2003_recall_all.push_back(bestMatch);
				//判断字符差异
				if (&matchCPlate&&bestMatch > 0.4f) {
					recoPlateCount++;
					int diff = util::levenshtein_distance(gtImgName, recoImgName);
					if (diff == 0) {
						zeroErrorRecoPlateCount++;
						oneErrorRecoPlateCount++;
						//recoPlateCount++;
					}
					else if (diff == 1)
					{
						oneErrorRecoPlateCount++;
						//recoPlateCount++;
					}
					else
					{
						errorRecoPlateCount++;
					}
					if (gtCPlate.getPlateChars().substr(0, 1) == matchCPlate.getPlateChars().substr(0, 1)) {
						chineseCorrectCount++;
					}
				}
				else
				{
					noDetectPlateCount++;
				}
			}
		}
		//计算查全率
		recall = std::accumulate(icdar2003_recall_all.begin(), icdar2003_recall_all.end(), 0.0) / icdar2003_recall_all.size();
		float fScore = 2 * pricise*recall / (pricise + recall);

		float zeroErrorRate = 0;
		float oneErrorRate = 0;
		float ChineseCorrectRate = 0;
		if (recoPlateCount != 0) {
			zeroErrorRate = float(zeroErrorRecoPlateCount) / recoPlateCount;
			oneErrorRate = float(oneErrorRecoPlateCount) / recoPlateCount;
			ChineseCorrectRate = float(chineseCorrectCount) / recoPlateCount;
		}

		std::stringstream img_ss(std::stringstream::in | std::stringstream::out);
		std::string time = util::getTimeString();
		img_ss << time << std::endl;
		int plateCount = Controler::getControler()->getImgTotalCount();
		img_ss << "图片总数:" << plateCount << "    ";
		img_ss << "车牌总数:" << gtPlateCount << "    ";
		img_ss << "未检出车牌总数:" << noDetectPlateCount << "    ";
		img_ss << "检出率:" << (float(gtPlateCount - noDetectPlateCount) / gtPlateCount) * 100 << "%" << std::endl;
		img_ss << "准确率:" << pricise * 100 << "%" << "    ";
		img_ss << "查全率:" << recall * 100 << "%" << "    ";
		img_ss << "F 值:" << fScore * 100 << "%" << std::endl;

		img_ss << "0字符差错正确率:" << zeroErrorRate * 100 << "%" << "    "<<"车牌数:"<<zeroErrorRecoPlateCount<<std::endl;
		img_ss << "1字符差错正确率:" << oneErrorRate * 100 << "%" << "    " <<"车牌数:"<<oneErrorRecoPlateCount << std::endl;
		img_ss << "中文字符正确率:" << ChineseCorrectRate * 100 << "%" << std::endl;
		img_ss << "总时间：" << allTime << "s      " << "平均执行时间：" << float(allTime) / plateCount << "s" << std::endl;
		std::string temp;
		temp = img_ss.str();
		std::ofstream out("result/accuracy.txt", std::ios::app);
		if (out) {
			out << temp;
		}
		out.close();
		qDebug() << temp.c_str();
	
	return 0;
}

int Score::getGroundTruth(std::string path, std::map<std::string, std::vector<CPlate>> &CPlateMap) {
	XMLNode::setGlobalOptions(XMLNode::char_encoding_UTF8);
	XMLNode xMainNode = XMLNode::openFileHelper(path.c_str(), "tagset");
	const int n = xMainNode.nChildNode("image");

	for (int i = 0; i < n; i++) {
		XMLNode imageNode = xMainNode.getChildNode("image", i);
		std::string imageName = imageNode.getChildNode("imageName").getText();
		//imageName = imageName.substr(imageName.find(":" )+1);

		std::vector<CPlate> plateVec;
		XMLNode taggedRectangles = imageNode.getChildNode("taggedRectangles");
		int m = taggedRectangles.nChildNode("taggedRectangle");
		for (size_t j = 0; j < m; j++)
		{
			XMLNode plateNode = taggedRectangles.getChildNode("taggedRectangle", j);

			int x = atoi(plateNode.getAttribute("x"));
			int y = atoi(plateNode.getAttribute("y"));
			int width = atoi(plateNode.getAttribute("width"));
			int height = atoi(plateNode.getAttribute("height"));
			int angle = atoi(plateNode.getAttribute("rotation"));
			std::string plateString = plateNode.getText();
			plateString = plateString.substr(plateString.find(":") + 1);
			if (width < height) {
				std::swap(width, height);
				angle = angle + 90;
			}

			//cv::RotatedRect rr(cv::Point2f(float(x), float(y)), cv::Size2f(float(width), float(height)),float(angle));
			cv::RotatedRect rr(cv::Point2f(float(x), float(y)), cv::Size2f(float(width), float(height)), (float)angle);
			CPlate tempCPlate;
			tempCPlate.plateRotaRect = rr;
			tempCPlate.setPlateChars(plateString);
			plateVec.push_back(tempCPlate);
		}
		CPlateMap[imageName] = plateVec;
	}
	return 1;
}
