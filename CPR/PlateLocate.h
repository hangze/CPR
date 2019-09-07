#pragma once

enum plateSize
{
	//BIG =1
	//MID =0
};
enum CharType { CHINESE,LETTER,LETTER_NUM };
	class CPlate;
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
		inline cv::Mat & getPlateImg() { return plateImg; };
		inline void setPlateChars(std::string plateString) { plateChars = plateString; };
		inline std::string getPlateChars() { return plateChars; };
		inline cv::RotatedRect getRotatedRect() {
			cv::RotatedRect tempRRect=plateRotaRect;
			float ratio = float(800) / srcImg.width;
			tempRRect.center.x = tempRRect.center.x*ratio;
			tempRRect.center.y = tempRRect.center.y * ratio;
			tempRRect.size.width = tempRRect.size.width*ratio;
			tempRRect.size.height = tempRRect.size.height*ratio;
			return tempRRect;
		};
		inline void setPlateRotaRect(cv::RotatedRect rr) {
			plateRotaRect = rr;
			float ratio = srcImg.width/float(800);
			plateRotaRect.center.x = plateRotaRect.center.x*ratio;
			plateRotaRect.center.y = plateRotaRect.center.y * ratio;
			plateRotaRect.size.width = plateRotaRect.size.width*ratio;
			plateRotaRect.size.height = plateRotaRect.size.height*ratio;
			
		};


		cv::Mat plateImg;
		cv::Mat plateBinaryImg;
		cv::Size srcImg;
		std::pair<CharType, cv::Mat> charCoding;
		std::vector<cv::Mat> plateCharacImgs;
		std::vector<cv::Rect> characterRects;
		std::vector<cv::Mat> characterCodeing;
		cv::RotatedRect plateRotaRect;
		std::string plateChars;
	private:

	};


	class PlateLocate
	{
	public:
		PlateLocate();
		~PlateLocate();
		bool startPR(const std::string &path, bool isBatchTest=false);
		void preProcess(const cv::Mat &preImg, cv::Mat &outputImg);//图像预处理
		void sobelResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect> &rotatedRects);//sobel方法查找车牌	
		void sobelFrtResearch(const cv::Mat &srcImg,const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelFrtRects, bool *hitArea);//第一次sobel搜索
		void sobelSecResearch(const cv::Mat &srcImg, const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects, bool *hitArea);//与第一次的区别时是形态学算子大小不同
		void sobelRefineResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects);//第三次jsobel精细搜索，
		void plateJudge(const cv::Mat &srcimg, std::vector<cv::RotatedRect> &rotatedRects);		
		int platecout=0;
		static std::string tempPath;
		cv::Size currSize;
		XMLNode xMainNode = XMLNode::createXMLTopNode("tagset");
		std::string resultPath = "D:/VS/CPR/CPR/img_test /Result.xml";
		std::map<std::string, std::vector<CPlate>> recoCPlateMap;
	private:
		//void 
		uchar findDdgeDeskew();
		void sobelOpert(const cv::Mat &srcImg, cv::Mat &outputImg, cv::Size blurSize);
		void kMean(cv::Mat srcImg, cv::Size blurSize, int clusterNum);
		void spatialOstu(cv::Mat &inputOutputImg, float persent=0.7,int grid_x=1, int grid_y=1);
		void spatialOstu1(cv::Mat &inputOutputImg, float persent = 0.7, int grid_x = 1, int grid_y = 1);
		void deskew();
		void findMappingVertical(const cv::Mat &srcImg, cv::Mat &outputImg);//垂直方向的精细化，将在第二次sobel搜索检测到倾斜时使用
		bool vertifyCharacterSizes(const cv::Rect &Rect);//判断矩形是否字符区域条件	
		bool vertifySizes(const cv::RotatedRect &rRect, float error,uchar mode=0);//判断矩形是否车牌区域条件	
		bool vertifySizesTemp(const cv::RotatedRect &rRect);
		cv::Mat srcImg;
		std::vector<CPlate> currPlates;
	};

	
