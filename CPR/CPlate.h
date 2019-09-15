#pragma once

enum CharType { CHINESE, LETTER, LETTER_NUM };

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
	 cv::RotatedRect getRotatedRect();
	 void setPlateRotaRect(cv::RotatedRect rr);

	cv::Mat srcImg;//原图
	cv::Mat plateImg;//车牌图像
	cv::Mat plateBinaryImg;//二值化后的车牌图像
	cv::Size srcSize;//未经处理的原图大小
	//std::pair<CharType, cv::Mat> charCoding;
	std::vector<cv::Mat> plateCharacImgs;//字符图像，未用到
	std::vector<cv::RotatedRect> characterRects;//字符矩形向量，参考坐标系是车牌图像坐标系，而非原图
	std::vector<cv::Mat> characterCodeing;//字符识别结果（非字符串，而是编码），未用到
	cv::RotatedRect plateRotaRect;//车牌矩形
	std::string plateChars;//字符识别结果
private:

};