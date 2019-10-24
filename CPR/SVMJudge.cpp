#include "stdafx.h"
#include "SVMJudge.h"
#include<opencv2/ml/ml.hpp>

//构造函数内创建SVM对象并设置参数
SVMJudge::SVMJudge()
{
	SVMPtr = cv::ml::SVM::create();
	SVMPtr->setType(cv::ml::SVM::C_SVC);//SVM类型
	SVMPtr->setKernel(cv::ml::SVM::LINEAR);//核函数，这里使用线性核
	SVMPtr->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, 100, 0.001));//迭代终止条件
	std::string modelpath = "D:/VS/CPR/model/svm.xml";
	SVMPtr = SVMPtr->load(modelpath);
}

SVMJudge::~SVMJudge()
{
	 
}



void SVMJudge::startTrain(std::string &positivePath, std::string &negativePath,std::string &saveModelPath,cv::Size Imgsize) {
	
	qDebug() << "startTrain";
	//获取正样本路径下所有jpg，png文件列表，后期应改为c++版本，此处用到了QT库，
	QDir dir(QString::fromStdString(positivePath));
	QStringList nameFiters;
	nameFiters << "*.jpg" << "*.png";
	QStringList posImgNameList = dir.entryList(nameFiters, QDir::Files | QDir::Readable, QDir::Name);
	
	//读取正样本文件并提取特征
	cv::Mat positiveFeature;
	cv::Mat posFeatures;
	cv::Mat posLabels;	

	cv::Mat srcImg;
	cv::Mat tempImg;
	for (int i = 0; i < posImgNameList.size(); i++) {
		srcImg = cv::imread(positivePath +"/"+ posImgNameList[i].toStdString());
		cv::GaussianBlur(srcImg, srcImg, cv::Size(3,3), 0);
		cv::resize(srcImg, tempImg, cv::Size(136, 36));
		cv::cvtColor(tempImg, tempImg, cv::COLOR_BGR2GRAY);
		extractFeature(tempImg, cv::Size(34, 9), positiveFeature);
		(tempImg, cv::Size(34, 9), positiveFeature);
		posFeatures.push_back(positiveFeature);
		posLabels.push_back(1);
		qDebug() << "read positive sample";
	}
	posFeatures.convertTo(posFeatures, CV_32FC1);
	posLabels.convertTo(posLabels, CV_32SC1);


	//获取负样本路径下所有jpg，png文件列表
	dir.setPath(QString::fromStdString(negativePath));
	QStringList negImgNameList = dir.entryList(nameFiters, QDir::Files | QDir::Readable, QDir::Name);
	//读取负样本并提取特征
	cv::Mat negativeFeature;
	cv::Mat negFeatures;
	cv::Mat negLabels;
	
	for (int i = 0; i < negImgNameList.size(); i++) {
		srcImg = cv::imread(negativePath +"/" + negImgNameList[i].toStdString());
	//	cv::GaussianBlur(srcImg, srcImg, cv::Size(3, 3), 0);
		cv::resize(srcImg, tempImg, cv::Size(136, 36));
		cv::cvtColor(tempImg, tempImg, cv::COLOR_BGR2GRAY);
		extractFeature(tempImg, cv::Size(34, 9), positiveFeature);
		posFeatures.push_back(positiveFeature);
		posLabels.push_back(0);
		qDebug() << "read negative sample";
	}

	posFeatures.convertTo(posFeatures, CV_32FC1);
	posLabels.convertTo(posLabels, CV_32SC1);
	cv::Ptr<cv::ml::TrainData> tData = cv::ml::TrainData::create(posFeatures, cv::ml::ROW_SAMPLE, posLabels);
	SVMPtr->train(tData);
	SVMPtr->save(saveModelPath+"/svm.xml");
	
}
bool SVMJudge::startJudge(const cv::Mat &srcImg,float &score) {
	
	qDebug() << SVMPtr->getVarCount();
	cv::Mat Feature;

	cv::Mat tempImg;
	//cv::GaussianBlur(srcImg, srcImg, cv::Size(3, 3), 0);
	cv::resize(srcImg, tempImg, cv::Size(136, 36));
	if (srcImg.channels() == 3) {
		cv::cvtColor(tempImg, tempImg, cv::COLOR_BGR2GRAY);
	}
	
	
	clahe->apply(tempImg, tempImg);
	//cv::imshow("prep", tempImg);
	//cv::waitKey(0);
	extractFeature(tempImg, cv::Size(34, 9), Feature);
	Feature.convertTo(Feature, CV_32FC1);
	score = SVMPtr->predict(Feature,cv::noArray(),cv::ml::StatModel::RAW_OUTPUT);
	if (score <0.3) {//原为0.3
		
		qDebug() << "plates"<<score;
		return true;
	}
	else
	{
		qDebug() << "no plates" << score;
		return false;
	}
}
cv::Mat SVMJudge::extractFeature(const cv::Mat &srcImg, cv::Size &cellSize, cv::Mat &featureVector) {
	LBP::ComputeLBPFeatureVector_256(srcImg, cellSize, featureVector);
	return featureVector;
}