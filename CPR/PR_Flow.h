#pragma once
#include"PlateLocate.h"
#include"CharsSegment.h"
#include"CNNRecognizer.h"
extern CPlate;
//extern class PlateLocate;
class PR_Flow
{
	
public:
	PR_Flow();
	~PR_Flow();
	bool startPR(std::string imgPath);
	void batchTest(std::vector<std::string> vecPath);
private:
	std::vector<CPlate> currCPlates;
	cv::Mat srcImg;
	cv::Size currSize;
	PlateLocate PLocate;
	CharsSegment charsSegment;
	CNNRecognizer *cnnRecognizer;
};

