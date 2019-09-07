#pragma once

/*****************************************************************************
*  @file     backupSource.h
*  @brief    保存一些测试过程中可行但被更优的方法代替的代码，备用
*  Details.
*
*  @author   HZH
*  @email    276095361@qq.com
*  @version  1.0.0.1
*  @date     2019.08.12
*  @license  GNU General Public License (GPL)
*
*----------------------------------------------------------------------------
*  Remark         : Description
*****************************************************************************/

/*采用Hough变换查找车牌字符的上下边缘，效果良好，目前只实现查找其实现，未矫正，在此方法基础上有重大改进，故弃之
void PlateLocate::sobelRefineResearch(const cv::Mat &inputImg, std::vector<cv::RotatedRect>&sobelSecRects) {
	std::vector<cv::Mat> extendImg = currPlates.getPlateImgs();
	std::vector<cv::Mat>::iterator iterBegin = extendImg.begin();
	cv::Mat sobelXImg;
	cv::Mat thresSobelXImg;
	cv::Mat grayImg;
	cv::Mat morphImg;
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(19, 1));
	//std::vector<cv::Vec4i> lines;
	std::vector<cv::Vec2f> lines;
	for (; iterBegin != extendImg.end(); iterBegin++) {

		cv::Mat tempImg;
		cv::resize(*iterBegin, tempImg, cv::Size(136, 36));
		cv::cvtColor(tempImg, grayImg, CV_BGR2GRAY);
		cv::Sobel(grayImg, sobelXImg, CV_16S, 1, 0);
		convertScaleAbs(sobelXImg, thresSobelXImg);
		cv::threshold(thresSobelXImg, thresSobelXImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
		cv::morphologyEx(thresSobelXImg, morphImg, cv::MORPH_CLOSE, element);
		cv::imshow("morph", morphImg);
		cv::copyMakeBorder(morphImg, morphImg, 3, 3, 2, 2, cv::BORDER_CONSTANT, cv::Scalar(0));
		cv::copyMakeBorder(tempImg, tempImg, 3, 3, 2, 2, cv::BORDER_CONSTANT, cv::Scalar(0));
		cv::Canny(morphImg, morphImg, 50, 90);
		cv::imshow("cnanny", morphImg);

		//采用霍夫变换查找上下直线，效果不错
		//cv::HoughLinesP(morphImg, lines, 1, CV_PI / 180, 20, 30, 5);//
		cv::HoughLines(morphImg, lines, 1, CV_PI / 180, 30);
		//cv::fitLine(morphImg, lines, CV_DIST_HUBER, 0, 0.01, 0.01);
		std::vector<cv::Vec2f>::const_iterator it = lines.begin();
		while (it != lines.end())
		{
			float rho = (*it)[0];
			float theta = (*it)[1];
			if (theta<CV_PI / 4.0 || theta>3.0*CV_PI / 4.0)
			{
				//直线与第一行的交叉点
				cv::Point pt1(static_cast<int>(rho / cos(theta)), 0.0);
				//直线与最后一行的交叉点
				cv::Point pt2(rho / cos(theta) - tempImg.rows*sin(theta) / cos(theta), tempImg.rows);
				cv::line(tempImg, pt1, pt2, cv::Scalar(0, 255, 0), 1);
			}
			else
			{
				cv::Point pt1(0, rho / sin(theta));
				cv::Point pt2(tempImg.cols, rho / sin(theta) - tempImg.cols*cos(theta) / sin(theta));
				cv::line(tempImg, pt1, pt2, cv::Scalar(0, 0, 255), 1);
			}
			it++;
		}
		
	//	for (size_t i = 0; i < lines.size(); i++)
	//	{
	//		cv::line(tempImg, cv::Point(lines[i][0], lines[i][1]),
	//			cv::Point(lines[i][2], lines[i][3]), cv::Scalar(0, 0, 255), 2, 8);
	//	}
		cv::imshow("houghline", tempImg);
		cv::waitKey(0);
	}
}
*/

/*测试sobel算子效果的代码
void testSobel() {
	cv::resize(*iterBegin, temgpImg, cv::Size(272, 72));
	cv::cvtColor(temgpImg, temgpImg, cv::COLOR_BGR2GRAY);
	cv::Sobel(temgpImg, sobelXImg, CV_8U, 1, 0);
	cv::Sobel(temgpImg, sobelYImg, CV_8U, 0, 1);
	cv::Sobel(temgpImg, sobelImg, CV_8U, 1, 1);

	//cv::imshow("prisobelXImg", sobelXImg);
	//cv::imshow("prisobelYImg", sobelYImg);
	//cv::imshow("prisobelImg", sobelImg);

	cv::Mat thresSobelXImg;
	cv::Mat thresSobelYImg;
	cv::Mat thresSobelImg;
	cv::threshold(sobelXImg, thresSobelXImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::threshold(sobelYImg, thresSobelYImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::threshold(sobelImg, thresSobelImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	cv::imshow("sobelXImg", thresSobelXImg);
	cv::imshow("sobelYImg", thresSobelYImg);
	cv::imshow("sobelImg", thresSobelImg);
	sobelImg = cv::abs(sobelXImg + sobelYImg);
	//cv::imshow("prinewsobel IMG", sobelImg);
	cv::threshold(sobelImg, sobelImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::imshow("xysobelIMG", sobelImg);

	cv::Sobel(temgpImg, sobelXImg, CV_16S, 1, 0);
	//cv::imshow("sobelXImg16s", sobelXImg);
	convertScaleAbs(sobelXImg, thresSobelXImg);
	cv::imshow("sobelXImg16sBinary", thresSobelXImg);
	cv::Sobel(temgpImg, sobelYImg, CV_16S, 0, 1);
	//cv::imshow("sobelYImg16s", sobelYImg);
	convertScaleAbs(sobelYImg, thresSobelYImg);
	cv::imshow("sobelYImg16sBinary", thresSobelYImg);

	addWeighted(thresSobelXImg, 0.5, thresSobelYImg, 0.5, 0, thresSobelImg);
	cv::imshow("addWeighted", thresSobelImg);

	cv::threshold(thresSobelXImg, thresSobelXImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::imshow("16ssobelXimgBinary", thresSobelXImg);

	cv::threshold(thresSobelYImg, thresSobelYImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::imshow("16ssobelYimgBinary", thresSobelYImg);
	cv::threshold(thresSobelImg, thresSobelImg, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	cv::imshow("16ssobelimgBinary", thresSobelImg);


	//cv::convert
	cv::waitKey(0);
}
*/