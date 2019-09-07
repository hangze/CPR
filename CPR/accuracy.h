#pragma once
#include "stdafx.h"
#include"PlateLocate.h"

int calcuAccuracy() {

	return	0;
}

int getGroundTruth(std::string path,std::map<std::string,std::vector<CPlate>> &CPlateMap) {
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
			tempCPlate.plateRotaRect=rr;
			tempCPlate.setPlateChars(plateString);
			plateVec.push_back(tempCPlate);
		}
		CPlateMap[imageName] = plateVec;
	}
	return 1;
}