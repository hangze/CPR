#pragma once
class CPlate;

class PlateLocate
{
public:
	PlateLocate();
	~PlateLocate();
	static bool startPR(std::string &path, CPlate &plate);

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
	cv::Mat plateImg;
	cv::RotatedRect rect;
private:

};