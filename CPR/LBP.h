#pragma once
//////////////////////////////////////////////////////////////////////////
//  LBP.h (2.0)
// 2015-6-30，by QQ
//
// Please contact me if you find any bugs, or have any suggestions.
// Contact:
//		Telephone:17761745857
//		Email:654393155@qq.com
//		Blog: http://blog.csdn.net/qianqing13579
//////////////////////////////////////////////////////////////////////////
// updated 2016-12-12 01:12:55 by QQ, LBP 1.1,GetMinBinary()函数修改为查找表，提高了计算速度
// updated 2016-12-13 14:41:58 by QQ, LBP 2.0,先计算整幅图像的LBP特征图，然后计算每个cell的LBP直方图
//#include "opencv2/opencv.hpp"
//#include<vector>

class LBP
{
public:

	// 计算基本的256维LBP特征向量
	static void ComputeLBPFeatureVector_256(const cv::Mat &srcImage, cv::Size cellSize, cv::Mat &featureVector);
	static void ComputeLBPImage_256(const cv::Mat &srcImage, cv::Mat &LBPImage);// 计算256维LBP特征图

	// 计算灰度不变+等价模式LBP特征向量(58种模式)
	static void ComputeLBPFeatureVector_Uniform(const cv::Mat &srcImage, cv::Size cellSize, cv::Mat &featureVector);
	static void ComputeLBPImage_Uniform(const cv::Mat &srcImage, cv::Mat &LBPImage);// 计算等价模式LBP特征图

	// 计算灰度不变+旋转不变+等价模式LBP特征向量(9种模式)
	static void ComputeLBPFeatureVector_Rotation_Uniform(const cv::Mat &srcImage, cv::Size cellSize, cv::Mat &featureVector);
	static void ComputeLBPImage_Rotation_Uniform(const cv::Mat &srcImage, cv::Mat &LBPImage); // 计算灰度不变+旋转不变+等价模式LBP特征图,使用查找表

	// Test
	static void Test();// 测试灰度不变+旋转不变+等价模式LBP
	static void TestGetMinBinaryLUT();

private:
	static void BuildUniformPatternTable(int *table); // 计算等价模式查找表
	static int GetHopCount(int i);// 获取i中0,1的跳变次数

	static void ComputeLBPImage_Rotation_Uniform_2(const cv::Mat &srcImage, cv::Mat &LBPImage);// 计算灰度不变+旋转不变+等价模式LBP特征图,不使用查找表
	static int ComputeValue9(int value58);// 计算9种等价模式
	static int GetMinBinary(int binary);// 通过LUT计算最小二进制 
	static uchar GetMinBinary(uchar *binary); // 计算得到最小二进制

};


