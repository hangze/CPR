#pragma once
#include"CPlate.h"
class CharsSegment
{
public:
	CharsSegment();
	~CharsSegment();
	void genernalSegment(std::vector<CPlate> &vecCPlate);
	void segmentUsingProjection(cv::Mat &srcImg, cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr);
	void segmentUsingRect(cv::Mat &srcImg, cv::RotatedRect& rr);
private:
	void thresInterface(cv::Mat &inputImg,cv::Mat &outputImg);
	void spatialOstu(cv::Mat &inputOutputImg, float persent, int grid_x, int grid_y);
};

