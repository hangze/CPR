#pragma once
#include <math.h>
#include"stdafx.h"


//#define DEBUG
#define DEBUG 1
//macro config
#define IDEA_WIDTH 800
#define IDEA_HIGHT 600
#define DEAFAULT_PLATE_WIDTH 136
#define DEAFAULT_PLATE_HIGHT 36	
#define DEAFAULT_GAUSIIBLUR_SIZE 5
#define DEAFAULT_MORPH_WIDTH 17
#define DEAFAULT_MORPH_HIGHT 3

#define DEAFAULT_SIZE_ERROR 0.4
#define DEAFAULT_SIZE_REFINE_ERROR 0.2//精定位时的判别偏差
#define DEAFAULT_SIZE_ASPECT 3.75//车牌大小默认长宽比，注意，与图纸数据比例3.17不同
#define DEAFAULT_SIZE_MINAREA 500 //
#define DEAFAULT_SIZE_MAX_AREA 27000 //目前的值由少量样本目测得来，注意，大小与图片大小(现长为800，比例固定)息息相关
#define DEAFAULT_TEMPIMG "D:\\VS\\CPR\\TEMPIMG\\"
#define DEAFAULT_GRID_X 8
#define DEAFAULT_GRID_Y 12

#define SHOW_IMAGE(imgName, debug) \
  if (debug) { \
    cv::namedWindow("imgName", cv::WINDOW_AUTOSIZE); \
    cv::imshow("imgName", imgName); \
	cv::waitKey(1000);\
}
    //cv::waitKey(0); \//
   // cv::destroyWindow("imgName"); \//
  //}

namespace util{



	

	void probilityThreshold(cv::Mat &inputGray, cv::Mat &outputGray, int percent);
	void contrastEnhance(cv::Mat &inputGray, cv::Mat &outputGray, int percent);
	void  clearUpDownBorder(const cv::Mat &binaryPlateImg, cv::Mat &outputPlateImg, int threshold);

	void ACE(cv::Mat &src, int C = 3, int n = 3, float MaxCG = 7.5);
	cv::Mat matrixWiseMulti(cv::Mat &m1, cv::Mat &m2);
	
	std::string getTimeString();
	//计算旋转矩形的安全正置矩形
	bool calcSafeRect(const cv::RotatedRect &roi_rect, const cv::Size &src,
		cv::Rect_<float> &safeBoundRect);

	std::string UtfToGbk(const char* utf8);
	
	void contrastEnhance(cv::Mat &inputGray,cv::Mat &outputGray,int percent);

	//utf转gbk

	static std::string UTF8ToGBK(const char* strUTF8);


	/*
GBK 转 UTF-8
*/
	static std::string GBKToUTF8(const char* strGBK);

	/*
   * returns the smaller of the two numbers
   */
	template<typename T>
	static T min(const T &v1, const T &v2) {
		return (v1 < v2) ? v1 : v2;
	}

	//求编辑距离函数
	template<class T>
	static unsigned int levenshtein_distance(const T &s1, const T &s2) {
		const size_t len1 = s1.size(), len2 = s2.size();
		std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);//长度以s2为准

		for (unsigned int i = 0; i < prevCol.size(); i++) prevCol[i] = i;
		for (unsigned int i = 0; i < len1; i++) {//对标准车牌中的每个字符，判断识别车牌是否有字符与之相同
			col[0] = i + 1;
			for (unsigned int j = 0; j < len2; j++)//看不懂
				col[j + 1] = util::min(
					util::min(prevCol[1 + j] + 1, col[j] + 1),
					prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
			col.swap(prevCol);
		}
		return prevCol[len2];//返回时已不是前一行，而是最后一行
	}

	
	//函数声明
void on_mouse(int EVENT, int x, int y, int flags, void* userdata);


/**
 * @brief debug模式输出图片上鼠标点选的点
 * detail description
 * @param img    要显示的图片
 * @param windowName    窗口名
 */
void getImgPoint(cv::Mat img, const std::string windowName);

/*函数功能：求两条直线交点*/
/*输入：两条Vec4f类型直线*/
/*返回：Point2i类型的点*/
cv::Point2i getCrossPoint(cv::Vec4f LineA, cv::Vec4f LineB, uchar addx = 0, uchar addy = 0);

void AdaptiveThereshold(cv::Mat src, cv::Mat &dst, char percent = 0);

//内部函数，得到鼠标事件并做出debug输出像素位置和像素值//
void on_mouse(int EVENT, int x, int y, int flags, void* userdata);

/**
 * @brief 裁剪旋转矩形区域
 * detail description
 * @param srcImg    原始图像//必须原始图像，调整过大小的原始图像也没关系
 * @param rRect     旋转矩形
 * @param outImg    输出图像，大小与选项矩形同， 已正置
 * @param extend	true则截取旋转矩形扩大后的图像
 */
void getRotatedRectArea(const cv::Mat &srcImg, cv::RotatedRect &rRect, cv::Mat &outImg, cv::Mat &transMat, bool extend = false);

//画栅格线
void drawGrid(const cv::Mat &srcImg, cv::Mat &outputImg, int gridx, int gridy);
double getThreshVal_Otsu_8u(const cv::Mat & _src, double &pSigma);
void Wallner(cv::Mat &src, cv::Mat &dst);



void spatialOstu(cv::Mat &inputOutputImg, int grid_x, int grid_y);
int GetHuangFuzzyThreshold(cv::Mat &img);//基于模糊集的阈值方法
int GetMinimumThreshold(cv::Mat &img); //基于双峰的阈值方法
}//namespace util


