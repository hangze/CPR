#pragma once
#include "CPlate.h"

class Score
{
public:
	Score();
	~Score();

	int recordCPlate(std::vector<CPlate>&currPlates, std::string imgPath);
	int calcuAccuracy(std::string imgPath, int allTime);
	int getGroundTruth(std::string path, std::map<std::string, std::vector<CPlate>> &CPlateMap);
private:
	XMLNode xMainNode = XMLNode::createXMLTopNode("tagset");
	std::map<std::string, std::vector<CPlate>>recoCPlateMap;
};

