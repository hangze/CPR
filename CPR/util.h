#pragma once
#include <math.h>
//#define DEBUG
#define SHOW_IMAGE(imgName, debug) \
  if (debug) { \
    cv::namedWindow("imgName", cv::WINDOW_AUTOSIZE); \
    cv::moveWindow("imgName", 50, 50); \
    cv::imshow("imgName", imgName); \
}
    //cv::waitKey(0); \//
   // cv::destroyWindow("imgName"); \//
  //}

//函数声明
void on_mouse(int EVENT, int x, int y, int flags, void* userdata);


/**
 * @brief debug模式输出图片上鼠标点选的点
 * detail description
 * @param img    要显示的图片
 * @param windowName    窗口名
 */
void getImgPoint(cv::Mat img, const std::string windowName) {
	
	cv::namedWindow(windowName);
	cv::setMouseCallback(windowName, on_mouse, &img);

	 
	while (1)
	{
		cv::imshow(windowName, img);
		cv::waitKey(40);
	}
}

//内部函数，得到鼠标事件并做出debug输出像素位置和像素值//
void on_mouse(int EVENT, int x, int y, int flags, void* userdata)
{
	cv::Mat hh;
	hh = *(cv::Mat*)userdata;
	cv::Point p(x, y);
	
	switch (EVENT)
	{
	case cv::EVENT_LBUTTONDOWN:
	{
		if(3==hh.channels()){
		uchar B = hh.at<cv::Vec3b>(p)[0];
		uchar G = hh.at<cv::Vec3b>(p)[1];
		uchar R = hh.at<cv::Vec3b>(p)[2];

		float max = std::max(std::max(B, G),B);
		float min = std::min(std::min(R, G), B);
		int h;
		uchar  S, V;
		if (R ==max)  h = ((G - B) / (max - min))*60;
		if (G == max) h = (2+( B-R) / (max - min))*60;
		if (B == max) h = (4+(R-G) / (max - min))*60;
		h /= 2;
		V = max*255;
		S = ((max - min) / (max))*255;
		
		qDebug()<< "x=" << x << ",y=" << y;
		qDebug()<<"b="<<B << "  g=" << G << "  r=" << R;
		qDebug() << "H=" << h << "  S=" << S << "  V=" << V;
		qDebug() << "gray=" << uchar((R * 299 + G * 587 + B * 114)/1000);
		}
		else
		{
			qDebug() << "x=" << x << ",y=" << y;
			qDebug() << hh.at<uchar>(p);
		}
		circle(hh, p, 2, cv::Scalar(255), 3);
	}
	break;
	}
}
