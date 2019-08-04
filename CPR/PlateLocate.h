#pragma once


	class CPlate;


	class PlateLocate
	{
	public:
		PlateLocate();
		~PlateLocate();
		static bool startPR(std::string &path, CPlate &currPlates);
		static void preProcess(const cv::Mat &preImg, cv::Mat &outputImg);//图像预处理
		static void sobelResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect> &rotatedRects);//sobel方法查找车牌
		static bool vertifySizes(const cv::RotatedRect &rRect,float error);//判断矩形是否车牌条件
		static void plateJudge(const cv::Mat &srcimg, std::vector<cv::RotatedRect> &rotatedRects, CPlate&currPlates);
		static int platecout;
		static std::string tempPath;
	};

	/**
	   * @brief 储存车牌信息
	   * 车牌图像
	   * 车牌矩形坐标
	   * 车牌字符识别结果
	 */
	class CPlate
	{
	public:
		CPlate();
		~CPlate();
		std::vector<cv::Mat> plateImgs;
		std::vector<cv::RotatedRect> rRects;
		std::vector<std::string> plateChars;
	private:

	};

