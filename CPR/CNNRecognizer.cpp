#include "stdafx.h"
#include "CNNRecognizer.h"
#include "PlateLocate.h"
#include "CPlate.h"
CNNRecognizer::CNNRecognizer(std::string &prototxt,std::string &caffemodel)
{
	net = cv::dnn::readNetFromCaffe(prototxt, caffemodel);//构造函数中读取模型
}



std::string CNNRecognizer::recognizeCharacter(cv::Mat &characterImg,CharType characterType) {
	if (characterImg.channels() == 3) { cv::cvtColor(characterImg, characterImg, CV_BGR2GRAY); }
	cv::Mat inputBlob = cv::dnn::blobFromImage(characterImg, 1 / 255.0, cv::Size(CHAR_INPUT_W, CHAR_INPUT_H), cv::Scalar(),false);
	net.setInput(inputBlob,"data");
	cv::Mat probMat=net.forward();
	std::string decode;
	decode = decodePlateNormal(chars_code, probMat, characterType);
	return decode;
}


std::string CNNRecognizer::decodePlateNormal(std::vector<std::string> mappingTable,cv::Mat probMat,CharType characterType) {
	std::string decode;
	float confidence=0;
	float *prob = (float *)probMat.data;
	if (characterType == CHINESE) {
		decode += mappingTable[std::max_element(prob, prob + 31) - prob];
		confidence += *std::max_element(prob, prob + 31);
	}
	if (characterType == LETTER) {
		decode += mappingTable[std::max_element(prob + 41, prob + 65) - prob];
		confidence += *std::max_element(prob, prob + 41);
	}
	if (characterType == LETTER_NUM) {
		decode += mappingTable[std::max_element(prob + 31, prob + 65) - prob];
		confidence += *std::max_element(prob + 31, prob + 65);
	}
	return decode;
}
CNNRecognizer::~CNNRecognizer()
{
}
