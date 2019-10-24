#pragma once
typedef cv::Mat label;
extern class CPlate;
extern enum CharType;
class GenernalRecognizer
{
public:
	cv::dnn::Net net;
	GenernalRecognizer();
	~GenernalRecognizer();

	virtual std::string recognizeCharacter(cv::Mat &characterImg, CharType characterType, double &totalconfidence) = 0;
	void SegmentBasedSequenceRecognition(std::vector<CPlate> &plate);
	
};

