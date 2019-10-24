#pragma once
#include"SVMJudge.h"
extern class CPlate;
	class PlateLocate
	{
	public:
		PlateLocate();
		~PlateLocate();
		bool locate(cv::Mat srcImg, std::vector<CPlate> &currCPlates);
		void preProcess(const cv::Mat &preImg, cv::Mat &outputImg);//图像预处理
		void sobelProcess(const cv::Mat&sobelImg, cv::Mat &outputImg);
		void sobelResearch(const cv::Mat &inputImg,std::vector<CPlate>&currCPlates);//sobel方法查找车牌
		void sobelResearch1(const cv::Mat &inputImg, std::vector<CPlate>&currCPlates);
		void sobelResearch3(const cv::Mat &inputImg, std::vector<CPlate>&currCPlates);
		void sobelFrtResearch(const cv::Mat &srcImg,const cv::Mat &inputImg , std::vector<CPlate>&currCPlates);//第一次sobel搜索
		void sobelFrtResearch1(const cv::Mat &srcImg, const cv::Mat &grayImg ,const cv::Mat &inputImg, std::vector<CPlate>&currCPlates);//第一次sobel搜索
		void sobelSecResearch(const cv::Mat &srcImg, const cv::Mat &inputImg, std::vector<CPlate>&currCPlates);//与第一次的区别时是形态学算子大小不同
		void sobelRefineResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects);//第三次jsobel精细搜索，
		void plateJudge(const cv::Mat &srcimg, std::vector<cv::RotatedRect> &rotatedRects);		
		bool jumpPointCount(const cv::Mat plateImg,uchar threshold);
		bool repeatRemoveUseNMS(std::vector<CPlate> vecCPlate,cv::RotatedRect );
		int platecout=0;
		
		static std::string tempPath;
		cv::Size currSize;
		float sizeI=0;
		bool isShow;
		XMLNode xMainNode = XMLNode::createXMLTopNode("tagset");
		std::string resultPath = "D:/VS/CPR/CPR/img_test /Result.xml";

	private:
		uchar findEdgeDeskew(std::vector<CPlate>&currCPlates);
		void sobelOpert(const cv::Mat &srcImg, cv::Mat &outputImg, cv::Size blurSize);
		void kMean(cv::Mat srcImg, cv::Size blurSize, int clusterNum);
		void spatialOstu(cv::Mat &inputOutputImg, float persent=0.7,int grid_x=1, int grid_y=1);
		void deskew();
		bool fineMappingVertical(const cv::Mat &srcImg, cv::Mat &outputImg,cv::Mat &transMat, cv::Mat emtireImg);//垂直方向的精细化，将在第二次sobel搜索检测到倾斜时使用
		void fineMapping(std::vector<CPlate> &vecCPlate);
		bool fineMappingHorizontal(const cv::Mat &plateImg, cv::Mat &outputThresImg,cv::Mat &transMat,cv::Mat emtireImg);
		double findThetaUsingRadon(const cv::Mat &plateImgThres);
		bool vertifyCharacterSizes(const cv::Rect &Rect, float minRatio, float maxRatio,int minArea,int maxArea);//判断矩形是否字符区域条件	
		bool vertifySizes(const cv::RotatedRect &rRect, float error,uchar mode=0);//判断矩形是否车牌区域条件	
		bool vertifySizesTemp(const cv::RotatedRect &rRect);
		void clearUpDownBorder(const cv::Mat &binaryPlateImg,cv::Mat &outputPlateImg,int threshold);
		cv::Mat srcImg;
		SVMJudge svm;
		cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();//默认参数

	};

	
