#pragma once
#include"SVMJudge.h"
extern class CPlate;
	class PlateLocate
	{
	public:
		PlateLocate();
		~PlateLocate();
		void batchTest(std::vector<std::string> vecPath);
		bool locate(cv::Mat srcImg, std::vector<CPlate> &currCPlates);
		void preProcess(const cv::Mat &preImg, cv::Mat &outputImg);//图像预处理
		void sobelResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect> &rotatedRects,std::vector<CPlate>&currCPlates);//sobel方法查找车牌	
		void sobelFrtResearch(const cv::Mat &srcImg,const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelFrtRects , std::vector<CPlate>&currCPlates, bool *hitArea);//第一次sobel搜索
		void sobelSecResearch(const cv::Mat &srcImg, const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects, std::vector<CPlate>&currCPlates,bool *hitArea);//与第一次的区别时是形态学算子大小不同
		void sobelRefineResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects);//第三次jsobel精细搜索，
		void plateJudge(const cv::Mat &srcimg, std::vector<cv::RotatedRect> &rotatedRects);		
		int platecout=0;
		static std::string tempPath;
		cv::Size currSize;
		XMLNode xMainNode = XMLNode::createXMLTopNode("tagset");
		std::string resultPath = "D:/VS/CPR/CPR/img_test /Result.xml";

	private:
		uchar findDdgeDeskew(std::vector<CPlate>&currCPlates);
		void sobelOpert(const cv::Mat &srcImg, cv::Mat &outputImg, cv::Size blurSize);
		void kMean(cv::Mat srcImg, cv::Size blurSize, int clusterNum);
		void spatialOstu(cv::Mat &inputOutputImg, float persent=0.7,int grid_x=1, int grid_y=1);
		void deskew();
		void findMappingVertical(const cv::Mat &srcImg, cv::Mat &outputImg);//垂直方向的精细化，将在第二次sobel搜索检测到倾斜时使用
		bool vertifyCharacterSizes(const cv::Rect &Rect);//判断矩形是否字符区域条件	
		bool vertifySizes(const cv::RotatedRect &rRect, float error,uchar mode=0);//判断矩形是否车牌区域条件	
		bool vertifySizesTemp(const cv::RotatedRect &rRect);
		cv::Mat srcImg;
		SVMJudge svm;

	};

	
