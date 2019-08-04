#pragma once
#include <math.h>
//#define DEBUG
#define SHOW_IMAGE(imgName, debug) \
  if (debug) { \
    cv::namedWindow("imgName", cv::WINDOW_AUTOSIZE); \
    cv::imshow("imgName", imgName); \
}
    //cv::waitKey(0); \//
   // cv::destroyWindow("imgName"); \//
  //}

namespace util{
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

/**
 * @brief 裁剪旋转矩形区域
 * detail description
 * @param srcImg    原始图像//必须原始图像
 * @param rRect     旋转矩形
 * @param outImg    输出图像，大小与选项矩形同， 已正置
 */
void getRotatedRectArea(const cv::Mat &srcImg,const cv::RotatedRect &rRect,cv::Mat &outImg) {

	cv::Point2f vertices[4];
	cv::RotatedRect rRectCopy = rRect;
	rRectCopy.points(vertices);
	if (rRectCopy.size.width < rRectCopy.size.height) {
		std::swap(rRectCopy.size.width, rRectCopy.size.height);
		std::swap(vertices[0], vertices[3]);
		std::swap(vertices[0], vertices[2]);
		std::swap(vertices[0], vertices[1]);
	}
	//将矩形点转换到原图大小的矩形点
	for (int i = 0; i < 4; i++) {
		vertices[i].x = vertices[i].x*srcImg.cols / 800;
		vertices[i].y = vertices[i].y*srcImg.cols / 800;
	}
	cv::Point2f dstPoint[3];
	dstPoint[0] = cv::Point2f(0, rRectCopy.size.height*srcImg.cols / 800);
	dstPoint[1] = cv::Point2f(0, 0);                                         
	dstPoint[2] = cv::Point2f(rRectCopy.size.width*srcImg.cols / 800, 0);
	cv::Mat transMat = cv::getAffineTransform(vertices, dstPoint);
	//cv::GaussianBlur(src)//此处可考虑使用高斯模糊
	cv::warpAffine(srcImg, outImg, transMat, cv::Size(rRectCopy.size.width*srcImg.cols / 800, rRectCopy.size.height*srcImg.cols / 800));

}


}//namespace util



