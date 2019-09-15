#include "stdafx.h"

#include "Controler.h"
#include "PR_Flow.h"
#include "CPlate.h"
#include"Score.h"


bool PR_Flow::startPR(std::string imgPath) {
	#ifdef DEBUG
		qDebug() << "startPR";
	#endif // DEBUG
	srcImg = cv::imread(imgPath);
	currSize = srcImg.size();
	
	if (!srcImg.data) {
		Controler::getControler()->ShowMessage("图片读取失败");
		return false;
	}
	PLocate.locate(srcImg, currCPlates);
	if (currCPlates.empty()) {
		//cv::imwrite(path + "false.jpg", srcImg);
		return false;//如果识别的车牌数为空，返回false
	}	
	
	charsSegment.genernalSegment(currCPlates);
	cnnRecognizer->SegmentBasedSequenceRecognition(currCPlates);
	
}

void PR_Flow::batchTest(std::vector<std::string> vecPath) {
	clock_t start, finish;
	
	float singleTime = 0;
	float allTime = 0;
	int currImgCount = Controler::getControler()->getCurrImgCount();//当前图片下标
	int imgTotalCount = Controler::getControler()->getImgTotalCount();//图片总数
	Score testScore;

	while (imgTotalCount > 0 && currImgCount < imgTotalCount&&currImgCount >= 0)
	{
		std::string stdpath = vecPath[currImgCount];
		Controler::getControler()->setCurrImgCount(currImgCount++);
		//std::string stdpath = path.toLocal8Bit();//转化为本地字符，与源文件编码方式相关

		start = clock();//计算时间
		startPR(stdpath);
		finish = clock();
		singleTime = (double)(finish - start) / CLOCKS_PER_SEC;
		qDebug() << "this function running time is " << singleTime << "s!";
		allTime += singleTime;
		singleTime = 0;
		testScore.recordCPlate(currCPlates, stdpath);
		currCPlates.clear();		
	}
	testScore.calcuAccuracy(vecPath[0],allTime);

}

PR_Flow::PR_Flow()
{
	std::string caffeModel = "D:/VS/CPR/model/CharacterRecognization.caffemodel";
	std::string prototext = "D:/VS/CPR/model/CharacterRecognization.prototxt";
	cnnRecognizer = new CNNRecognizer(prototext, caffeModel);
	
}


PR_Flow::~PR_Flow()
{
	delete cnnRecognizer;
}
