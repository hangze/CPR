#include"stdafx.h"
#pragma once
#include"util.h"


namespace util {

	std::string getTimeString() {
		time_t t = time(NULL);
		struct tm *localt = localtime(&t);
		std::stringstream ssTime;
		ssTime << localt->tm_year + 1900 << "-";
		ssTime << localt->tm_mon + 1 << "-";
		ssTime << localt->tm_mday << " ";
		ssTime << localt->tm_hour << ":";
		ssTime << localt->tm_min << ":";
		ssTime << localt->tm_sec;
		//return "1";
		std::string temp;
		temp = ssTime.str();
		return temp;
	}
	//计算旋转矩形的安全正置矩形
	bool calcSafeRect(const cv::RotatedRect &roi_rect, const cv::Size &src,
		cv::Rect_<float> &safeBoundRect) {
		cv::Rect_<float> boudRect = roi_rect.boundingRect();

		float tl_x = boudRect.x > 0 ? boudRect.x : 0;
		float tl_y = boudRect.y > 0 ? boudRect.y : 0;

		float br_x = boudRect.x + boudRect.width < src.width
			? boudRect.x + boudRect.width - 1
			: src.width - 1;
		float br_y = boudRect.y + boudRect.height < src.height
			? boudRect.y + boudRect.height - 1
			: src.height - 1;

		float roi_width = br_x - tl_x;
		float roi_height = br_y - tl_y;

		if (roi_width <= 0 || roi_height <= 0) return false;

		//  a new rect not out the range of mat



		safeBoundRect = cv::Rect_<float>(tl_x, tl_y, roi_width, roi_height);

		return true;
	}


	std::string UtfToGbk(const char* utf8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
		len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
		if (wstr) delete[] wstr;
		return str;
	}



	//utf转gbk

	static std::string UTF8ToGBK(const char* strUTF8)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
		wchar_t* wszGBK = new wchar_t[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char* szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
		std::string strTemp(szGBK);

		if (wszGBK) delete[] wszGBK;
		if (szGBK) delete[] szGBK;

		return strTemp;
	}


	/*
GBK 转 UTF-8
*/
	static std::string GBKToUTF8(const char* strGBK)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* str = new char[len + 1];
		memset(str, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
		std::string strTemp = str;

		if (wstr) delete[] wstr;
		if (str) delete[] str;

		return strTemp;
	}


	

	void Wallner(cv::Mat &src, cv::Mat &dst)
	{
		/*

		* pn = 当前点的灰度值

		* s = 图片宽度/n （n = 8时效果最好）

		* t = 比例阈值

		* 公式：g(n) = g(n-1) * (1-1/s) + p(n)

		*/

		int t = -5;

		int s = src.cols >> 2;

		const int S = 9;

		const int power2S = 1 << S;//加速因子

		int factor = power2S * (100 - t) / (100 * s);

		/*使用初始值127*s *s是因为 原先算法采用均值也就是fn 是前n个像素之和

		这次算法优化为与当前点越相邻对其影响越大的思路*/

		int gn = 127 * s;

		int q = power2S - power2S / s;

		int pn, hn;

		int *prev_gn = NULL;//前一行各点像素值

		//Mat dst = Mat::zeros(src.size(), CV_8UC1);

		prev_gn = new int[src.cols];

		for (int i = 0; i < src.cols; i++)

			prev_gn[i] = gn;

		uchar * scanline = NULL;

		for (int i = 0; i < src.rows; i++)

		{

			scanline = src.ptr<uchar>(i);

			for (int j = 0; j < src.cols; j++)//从左向右遍历

			{

				pn = scanline[j];

				gn = ((gn * q) >> S) + pn;

				hn = (gn + prev_gn[j]) >> 1;

				prev_gn[j] = gn;

				pn < (hn * factor) >> S ? dst.at<uchar>(i, j) = 0 : dst.at<uchar>(i, j) = 255;

			}

			i++;

			if (i == src.rows)

				break;

			scanline = src.ptr<uchar>(i);

			for (int j = src.cols - 1; j >= 0; j--)//从右向左遍历

			{

				pn = scanline[j];

				gn = ((gn * q) >> S) + pn;

				hn = (gn + prev_gn[j]) >> 1;

				prev_gn[j] = gn;

				pn < (hn * factor) >> S ? dst.at<uchar>(i, j) = 0 : dst.at<uchar>(i, j) = 255;

			}

		}



	}


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


	/*函数功能：求两条直线交点*/
	/*输入：两条Vec4f类型直线*/
	/*返回：Point2i类型的点*/
	cv::Point2i getCrossPoint(cv::Vec4f LineA, cv::Vec4f LineB, uchar addx, uchar addy)
	{
		double k1, k2, b1, b2;
		k1 = static_cast<double>(LineA[1] / LineA[0]); //求出LineA斜率
		k2 = static_cast<double>(LineB[1] / LineB[0]); //求出LineB斜率
		b1 = static_cast<double>(LineA[3] - k1 * LineA[2]);
		b2 = static_cast<double>(LineB[3] - k2 * LineB[2]);
		cv::Point crossPoint;
		crossPoint.x = static_cast<uchar>((b2 - b1) / (k1 - k2) + addx);
		crossPoint.y = static_cast<uchar>(k1*(b2 - b1) / (k1 - k2) + b1 + addy);
		return crossPoint;
	}


	void AdaptiveThereshold(cv::Mat src, cv::Mat &dst, char percent)
	{
		if (src.type() == CV_8UC3) {
			cvtColor(src, dst, CV_BGR2GRAY);
		}

		int x1, y1, x2, y2;
		int count = 0;
		long long sum = 0;
		int S = 13;  //划分区域的大小S*S,S越大，边缘越粗
		//int T = 2;        //现用percent代替
		/*百分比，用来最后与阈值的比较。原文：If the value of the current pixel is t percent less than this average
							then it is set to black, otherwise it is set to white.*/
		int W = dst.cols;
		int H = dst.rows;
		long long **Argv;
		Argv = new long long*[dst.rows];
		for (int ii = 0; ii < dst.rows; ii++)
		{
			Argv[ii] = new long long[dst.cols];
		}
		for (int i = 0; i < W; i++)
		{
			sum = 0;
			for (int j = 0; j < H; j++)
			{
				sum += dst.at<uchar>(j, i);
				if (i == 0)
					Argv[j][i] = sum;
				else
					Argv[j][i] = Argv[j][i - 1] + sum;
			}
		}

		for (int i = 0; i < W; i++)
		{
			for (int j = 0; j < H; j++)
			{
				x1 = i - S / 2;
				x2 = i + S / 2;
				y1 = j - S / 2;
				y2 = j + S / 2;
				if (x1 < 0)
					x1 = 0;
				if (x2 >= W)
					x2 = W - 1;
				if (y1 < 0)
					y1 = 0;
				if (y2 >= H)
					y2 = H - 1;
				count = (x2 - x1)*(y2 - y1);
				sum = Argv[y2][x2] - Argv[y1][x2] - Argv[y2][x1] + Argv[y1][x1];
				if ((long long)(dst.at<uchar>(j, i)*count) < (long long)sum*(100 - percent) / 100)
					dst.at<uchar>(j, i) = 0;
				else
					dst.at<uchar>(j, i) = 255;
			}
		}
		for (int i = 0; i < dst.rows; ++i)
		{
			delete[] Argv[i];
		}
		delete[] Argv;

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
			if (3 == hh.channels()) {
				uchar B = hh.at<cv::Vec3b>(p)[0];
				uchar G = hh.at<cv::Vec3b>(p)[1];
				uchar R = hh.at<cv::Vec3b>(p)[2];

				float max = std::max(std::max(B, G), B);
				float min = std::min(std::min(R, G), B);
				int h;
				uchar  S, V;
				if (R == max)  h = ((G - B) / (max - min)) * 60;
				if (G == max) h = (2 + (B - R) / (max - min)) * 60;
				if (B == max) h = (4 + (R - G) / (max - min)) * 60;
				h /= 2;
				V = max * 255;
				S = ((max - min) / (max)) * 255;

				qDebug() << "x=" << x << ",y=" << y;
				qDebug() << "b=" << B << "  g=" << G << "  r=" << R;
				qDebug() << "H=" << h << "  S=" << S << "  V=" << V;
				qDebug() << "gray=" << uchar((R * 299 + G * 587 + B * 114) / 1000);
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
	 * @param srcImg    原始图像//必须原始图像，调整过大小的原始图像也没关系
	 * @param rRect     旋转矩形
	 * @param outImg    输出图像，大小与选项矩形同， 已正置
	 * @param extend	true则截取旋转矩形扩大后的图像
	 */
	void getRotatedRectArea(const cv::Mat &srcImg, const cv::RotatedRect &rRect, cv::Mat &outImg, bool extend) {

		float widthExtend = 1;
		float hightExtend = 1;
		cv::RotatedRect rRectCopy = rRect;
		float ratio = float(rRectCopy.size.width) / rRectCopy.size.height;
		if (extend) {
			widthExtend = 1.6;
			hightExtend = 1.2;
			//根据旋转矩形的长宽比进行扩大		
			if (ratio < 1) {
				ratio = 1 / ratio;
				if (ratio < 3.7)rRectCopy.size.height = rRectCopy.size.height *(8.2 / ratio - 1);//原为7.8
				if (rRectCopy.angle < -10 || rRectCopy.angle>-80) {
					rRectCopy.size.width += 16;
				}
				else
				{
					rRectCopy.size.width += 5;
				}
			}
			else
			{
				if (ratio < 3.7)rRectCopy.size.width = rRectCopy.size.width *(8.2 / ratio - 1);
				if (rRectCopy.angle<-10 || rRectCopy.angle>-80) { rRectCopy.size.height += 16; }
				else
				{
					rRectCopy.size.height += 5;
				}
			}
		}
		cv::Point2f vertices[4];
		rRectCopy.points(vertices);
		if (rRectCopy.size.width < rRectCopy.size.height) {
			std::swap(rRectCopy.size.width, rRectCopy.size.height);
			std::swap(vertices[0], vertices[3]);
			std::swap(vertices[0], vertices[2]);
			std::swap(vertices[0], vertices[1]);
		}
		//将矩形点转换到传入原图大小的矩形点
		for (int i = 0; i < 4; i++) {
			vertices[i].x = vertices[i].x*srcImg.cols / IDEA_WIDTH;
			vertices[i].y = vertices[i].y*srcImg.cols / IDEA_WIDTH;
		}
		cv::Point2f dstPoint[3];
		dstPoint[0] = cv::Point2f(0, rRectCopy.size.height*srcImg.cols / IDEA_WIDTH);
		dstPoint[1] = cv::Point2f(0, 0);
		dstPoint[2] = cv::Point2f(rRectCopy.size.width*srcImg.cols / IDEA_WIDTH, 0);
		cv::Mat transMat = cv::getAffineTransform(vertices, dstPoint);
		//cv::GaussianBlur(src)//此处可考虑使用高斯模糊
		cv::warpAffine(srcImg, outImg, transMat, cv::Size(rRectCopy.size.width*srcImg.cols / IDEA_WIDTH, rRectCopy.size.height*srcImg.cols / IDEA_WIDTH));

	}

	//画栅格线
	void drawGrid(const cv::Mat &srcImg, cv::Mat &outputImg, int gridx, int gridy) {

		outputImg = srcImg.clone();
		for (int i = 1; i < gridy; i++) {
			cv::line(outputImg, cv::Point(0, outputImg.size().height*i / gridy), cv::Point(800, outputImg.size().height*i / gridy), cv::Scalar(255, 255, 255), 3);
		}
		for (int i = 1; i < gridx; i++) {
			cv::line(outputImg, cv::Point(800 * i / gridx, 0), cv::Point(800 * i / gridx, outputImg.size().height), cv::Scalar(255, 255, 255), 3);
		}
		//Controler::getControler()->showImg(outputImg);
	}

	//获取图像的ostu阈值
	double getThreshVal_Otsu_8u(const cv::Mat & _src, double &pSigma)
	{
	cv:; assert(_src.type() == CV_8UC1);
		cv::Size size = _src.size();
		int step = (int)_src.step;
		if (_src.isContinuous())
		{
			size.width *= size.height;
			size.height = 1;
			step = size.width;
		}


		const int N = 256;
		int i, j, h[N] = { 0 };
		for (i = 0; i < size.height; i++)
		{
			const uchar* src = _src.ptr() + step * i;
			j = 0;
#if CV_ENABLE_UNROLLED
			for (; j <= size.width - 4; j += 4)
			{
				int v0 = src[j], v1 = src[j + 1];
				h[v0]++; h[v1]++;
				v0 = src[j + 2]; v1 = src[j + 3];
				h[v0]++; h[v1]++;
			}
#endif
			for (; j < size.width; j++)
				h[src[j]]++;
		}

		double mu = 0, scale = 1. / (size.width*size.height);
		for (i = 0; i < N; i++)
			mu += i * (double)h[i];

		mu *= scale;
		double mu1 = 0, q1 = 0;
		double max_sigma = 0, max_val = 0;

		for (i = 0; i < N; i++)
		{
			double p_i, q2, mu2, sigma;

			p_i = h[i] * scale;
			mu1 *= q1;
			q1 += p_i;
			q2 = 1. - q1;

			if (std::min(q1, q2) < FLT_EPSILON || std::max(q1, q2) > 1. - FLT_EPSILON)
				continue;

			mu1 = (mu1 + i * p_i) / q1;
			mu2 = (mu - q1 * mu1) / q2;
			sigma = q1 * q2*(mu1 - mu2)*(mu1 - mu2);
			if (sigma > max_sigma)
			{
				max_sigma = sigma;
				max_val = i;

			}
		}
		pSigma = max_sigma;
		return max_val;
	}

}//namespace util


