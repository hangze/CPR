#pragma once
#include"LBP.h"
class SVMJudge
{
public:
	SVMJudge();
	~SVMJudge(); 
	inline cv::Mat extractFeature(const cv::Mat &srcImg,cv::Size &cellSize,cv::Mat &featureVector);
	bool startJudge(const cv::Mat &srcImg);
	void startTrain(std::string &positivePath, std::string &negativePath, std::string &saveModelPath, cv::Size imgSize);

	inline void changeParam();
	cv::Ptr<cv::ml::SVM>  SVMPtr;
private:
	
};

