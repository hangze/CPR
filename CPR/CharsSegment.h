#pragma once
#include"CPlate.h"
#include"CNNRecognizer.h"

class CharsSegment
{
public:
	CharsSegment();
	~CharsSegment();
	void genernalSegment(std::vector<CPlate> &vecCPlate);
	void segmentUsingProjection(cv::Mat &srcImg, cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr);
	void segmentUsingHybrid(cv::Mat &plateImg, cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr);
	void segmentUsingFindRect(cv::Mat &plateImg, cv::Mat &outputThresImg, std::vector<cv::RotatedRect>&rr);
	std::string tempPath = "D:/VS/CPR/recoPlate1/";
private:
	void binaryInterface(cv::Mat &inputImg,cv::Mat &outputImg);
	void spatialOstu(cv::Mat &inputOutputImg, float persent, int grid_x, int grid_y);
	bool vertifyCharacterSizes(const cv::Rect &Rect);
	bool compX(const cv::Rect &rect1, const cv::Rect &rect2) {
		if (rect1.x < rect2.x) {
			return true;
		}
		else
		{
			return false;
		}
	}
	bool compWidth(const cv::Rect &rect1, const cv::Rect &rect2);

	CNNRecognizer *cnnRecognizer;

	struct SortDesc	{		bool operator() (const cv::Rect &rect1, const cv::Rect &rect2)		{			return rect1.x < rect2.x;		}	};	struct SortWidth	{		bool operator() (const cv::Rect &rect1, const cv::Rect &rect2)		{			return rect1.width > rect2.width;		}	};
};
