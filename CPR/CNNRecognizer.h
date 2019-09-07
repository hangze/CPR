#pragma once
#include "GenernalRecognizer.h"
extern enum CharType;
class CNNRecognizer:public GenernalRecognizer
{
public:
	const int CHAR_INPUT_W=14;
	const int CHAR_INPUT_H = 30;
	CNNRecognizer(std::string &prototxt,std::string &caffemodel);
	std::string recognizeCharacter(cv::Mat &characterImg,CharType characterType);
	~CNNRecognizer();
	std::vector<std::string> chars_code{ "京","沪","津","渝","冀","晋","蒙","辽","吉","黑","苏","浙","皖","闽","赣","鲁","豫","鄂","湘","粤","桂","琼","川","贵","云","藏","陕","甘","青","宁","新","0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","J","K","L","M","N","P","Q","R","S","T","U","V","W","X","Y","Z" };

private:
	cv::dnn::Net net;
	std::string decodePlateNormal(std::vector<std::string>mappingTable,cv::Mat probMat,CharType characterType);
};

