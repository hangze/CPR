#include"stdafx.h"
#pragma once
#include"util.h"


namespace util {
	
	void stretchImage(cv::Mat &img, double s = 0.005,int  bins = 2000) {
	//	cv::HistogramCostExtractor
		//	cv::km
	}


	
	int GetHuangFuzzyThreshold(cv::Mat &img)
	{	

		int histGram[256] = { 0 };
		if (img.channels() == 3) {
			cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
		}
		int nc = img.cols;
		int nr = img.rows;
		if (img.isContinuous()) {

			nc *= nr;
			nr = 1;
		}

		for (int i = 0; i < nr; i++) {
			uchar *indata = img.ptr<uchar>(i);
			for (int j = 0; j < nc; j++) {
				histGram[indata[j]]++;
			}
		}


		int X, Y;
		int First, Last;
		int Threshold = -1;
		double BestEntropy = static_cast<double>(65536), Entropy;
		//   找到第一个和最后一个非0的色阶值
		for (First = 0; First < 256 && histGram[First] == 0; First++);
		for (Last = 256 - 1; Last > First && histGram[Last] == 0; Last--);
		if (First == Last) return First;                // 图像中只有一个颜色
		if (First + 1 == Last) return First;            // 图像中只有二个颜色

		// 计算累计直方图以及对应的带权重的累计直方图
		std::vector<int> S;
		S.resize(Last + 1,0);
		std::vector<int> W;
		W.resize(Last + 1, 0);	          // 对于特大图，此数组的保存数据可能会超出int的表示范围，可以考虑用long类型来代替
		S[0] = histGram[0];

		for (Y = First > 1 ? First : 1; Y <= Last; Y++)
		{
			S[Y] = S[Y - 1] + histGram[Y];
			W[Y] = W[Y - 1] + Y * histGram[Y];
		}

		// 建立公式（4）及（6）所用的查找表
		std::vector<double>  Smu;
		Smu.resize(Last + 1 - First,0);
		for (Y = 1; Y < Smu.size(); Y++)
		{
			double mu = 1 / (1 + (double)Y / (Last - First));               // 公式（4）
			Smu[Y] = -mu * std::log(mu) - (1 - mu) * std::log(1 - mu);      // 公式（6）
		}

		// 迭代计算最佳阈值
		for (Y = First; Y <= Last; Y++)
		{
			Entropy = 0;
			int mu = (int)std::round((double)W[Y] / S[Y]);             // 公式17
			for (X = First; X <= Y; X++)
				Entropy += Smu[std::abs(X - mu)] * histGram[X];
			mu = (int)std::round((double)(W[Last] - W[Y]) / (S[Last] - S[Y]));  // 公式18
			for (X = Y + 1; X <= Last; X++)
				Entropy += Smu[std::abs(X - mu)] * histGram[X];       // 公式8
			if (BestEntropy > Entropy)
			{
				BestEntropy = Entropy;      // 取最小熵处为最佳阈值
				Threshold = Y;
			}
		}
		return Threshold*1.05;
	}


	//判断是否为双峰
	bool IsDimodal(double *HistGram)       // 检测直方图是否为双峰的
	{
		// 对直方图的峰进行计数，只有峰数位2才为双峰 
		int Count = 0;
		for (int Y = 1; Y < 255; Y++)
		{
			if (HistGram[Y - 1] < HistGram[Y] && HistGram[Y + 1] < HistGram[Y])
			{
				Count++;
				if (Count > 2) return false;
			}
		}
		if (Count == 2)
			return true;
		else
			return false;
	}



	//基于双峰谷底最小值的阈值方法
	int GetMinimumThreshold(cv::Mat &img)
	{

		int histGram[256] = {0};
		if (img.channels() == 3) {
			cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
		}
		int nc = img.cols;
		int nr = img.rows;
		if (img.isContinuous()) { 
			
			nc *= nr;
			nr = 1;
		}
		
		for (int i = 0; i < nr; i++) {
			uchar *indata = img.ptr<uchar>(i);
			for (int j = 0; j < nc; j++) {
				histGram[indata[j]]++;
			}
		}

		int Y, Iter = 0;
		double HistGramC[256];           // 基于精度问题，一定要用浮点数来处理，否则得不到正确的结果
		double HistGramCC[256];          // 求均值的过程会破坏前面的数据，因此需要两份数据
		for (Y = 0; Y < 256; Y++)
		{
			HistGramC[Y] = histGram[Y];
			HistGramCC[Y] = histGram[Y];
		}

		// 通过三点求均值来平滑直方图
		while (IsDimodal(HistGramCC) == false)                                        // 判断是否已经是双峰的图像了      
		{
			HistGramCC[0] = (HistGramC[0] + HistGramC[0] + HistGramC[1]) / 3;                 // 第一点
			for (Y = 1; Y < 255; Y++)
				HistGramCC[Y] = (HistGramC[Y - 1] + HistGramC[Y] + HistGramC[Y + 1]) / 3;     // 中间的点
			HistGramCC[255] = (HistGramC[254] + HistGramC[255] + HistGramC[255]) / 3;         // 最后一点
		//	System.Buffer.BlockCopy(HistGramCC, 0, HistGramC, 0, 256 * sizeof(double));
			memcpy(HistGramC, HistGramCC, 256*sizeof(double));
			Iter++;
			if (Iter >= 1000) return -1;                                                   // 直方图无法平滑为双峰的，返回错误代码
		}
		
		
		/*
		// 阈值极为两峰之间的最小值 
		bool Peakfound = false;
		for (Y = 1; Y < 255; Y++)
		{
			if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peakfound = true;
			if (Peakfound == true && HistGramCC[Y - 1] >= HistGramCC[Y] && HistGramCC[Y + 1] >= HistGramCC[Y])
				return Y - 1;
		}
		return -1;
		*/

		
		//阈值为两峰直接的中间值
		int Index = 0;
		int Peak[2] = { 0 };
		for (Y = 1, Index = 0; Y < 255; Y++)
			if (HistGramCC[Y - 1] < HistGramCC[Y] && HistGramCC[Y + 1] < HistGramCC[Y]) Peak[Index++] = Y - 1;
		return ((Peak[0] + Peak[1]) / 2);

	}
	


	void spatialOstu(cv::Mat &inputOutputImg, int grid_x, int grid_y) {
		cv::Mat &src = inputOutputImg;
		double globalSigma;
		if (src.channels() == 3) {
			cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);
		}
		double globalThreshVal = util::getThreshVal_Otsu_8u(src, globalSigma);
		int width = src.cols / grid_x;
		int height = src.rows / grid_y;
		std::vector<double> local_globalSigma;

		// iterate through grid

		double sigma;
		cv::Mat src_cellLine = cv::Mat(src, cv::Range(14, 24), cv::Range(10, 60));
		double cellThreshVal = util::getThreshVal_Otsu_8u(src_cellLine, sigma);//ostu阈值法
	//	double cellThreshVal = util::GetMinimumThreshold(src_cellLine);//基于双峰
	//	double cellThreshVal = util::GetHuangFuzzyThreshold(src_cellLine);//基于模糊集

		src_cellLine = cv::Mat(src, cv::Range(14, 24), cv::Range(60, 120));
		double cellThreshVal2 = util::getThreshVal_Otsu_8u(src_cellLine, sigma);//ostu阈值法
	//	double cellThreshVal2 = util::GetMinimumThreshold(src_cellLine);//基于双峰
	//	double cellThreshVal2 = util::GetHuangFuzzyThreshold(src_cellLine);//基于模糊集

		for (int j = 0; j < grid_y; j++) {

			cv::Mat src_cellLine = cv::Mat(src, cv::Range(18, 20), cv::Range(30, 100));
		//	double cellThreshVal = util::getThreshVal_Otsu_8u(src_cellLine, sigma);
			for (int i = 0; i < grid_x; i++) {
				
				cv::Mat src_cell = cv::Mat(src, cv::Range(j * height, (j + 1) * height), cv::Range(i * width, (i + 1) * width));
				
				cv::imshow("src_cell", src_cell);
				if (i >0 ) {
					cv::threshold(src_cell, src_cell, cellThreshVal2, 255, CV_THRESH_BINARY);
				}
				else
				{
					cv::threshold(src_cell, src_cell, cellThreshVal, 255, CV_THRESH_BINARY);
				}

				

				//cv::imshow("after src_cell", src_cell);
				//cv::waitKey(0);
			}
		}
	}



	cv::Mat matrixWiseMulti(cv::Mat &m1, cv::Mat &m2) {

		cv::Mat dst = m1.mul(m2);

		return dst;

	}





	/**
	 * @brief 概率阈值化
	 * percent占比内的灰度值较小的置0，其他置255
	 * @param inputGray    输入灰度图
	 * @param outputGray    输出二值图
	 * @param percent    百分比，该占比内的左边界灰度级全部置0，其他灰度级置255
	 * @return parameter description
	 *     -<em>false</em> fail
	 *     -<em>true</em> succeed
	 */
	void probilityThreshold(cv::Mat &inputGray, cv::Mat &outputGray, int percent) {
		if (inputGray.channels() == 3)cv::cvtColor(inputGray, inputGray, cv::COLOR_BGR2GRAY);
		int nc = inputGray.cols;
		int nr = inputGray.rows;
		int hist[256] = { 0 };
		if (inputGray.isContinuous()) {
			nc = nc * nr;
			nr = 1;
		}
		//计算直方图
		for (int i = 0; i < nr; i++) {
			uchar  *rowdata = inputGray.ptr<uchar>(i);
			for (int j = 0; j < nc; j++) {
				hist[rowdata[j]]++;
			}
		}
		//计算概率直方图
		int totalNum = nc;
		float probability[256] = { 0 };
		probability[0] = float(hist[0]) / nc;
		for (int i = 1; i < 256; i++) {
			probability[i] = probability[i - 1] + float(hist[i]) / nc;
		}

		
		int minGray;
		for (int i = 0; i < 256; i++) {
			if (probability[i] > percent / 100.0) {
				minGray = i;
				break;
			}
		}

		//修改图像灰度
		outputGray = inputGray.clone();
		for (int i = 0; i < nr; i++) {
			uchar  *rowdata = outputGray.ptr<uchar>(i);
			for (int j = 0; j < nc; j++) {
				if (rowdata[j] <= minGray) {
					rowdata[j] = 0;
				}
				else if (rowdata[j] >minGray)
				{
					rowdata[j] = 255;
				}
				
			}
		}
	}







	/**
	 * @brief 对比度增强
	 * percent占比内的左右边界灰度级，置为0或255,并拉伸
	 * @param inputGray    输入灰度图
	 * @param outputGray    输出灰度图
	 * @param percent    百分比，该占比内的左右边界灰度级，置为0或255
	 * @return parameter description
	 *     -<em>false</em> fail
	 *     -<em>true</em> succeed
	 */
	void contrastEnhance(cv::Mat &inputGray, cv::Mat &outputGray, int percent) {
		if (inputGray.channels() == 3)cv::cvtColor(inputGray, inputGray, cv::COLOR_BGR2GRAY);
		int nc = inputGray.cols;
		int nr = inputGray.rows;
		int hist[256] = { 0 };
		if (inputGray.isContinuous()) {
			nc = nc * nr;
			nr = 1;			
		}
		//计算直方图
		for (int i = 0; i < nr; i++) {
			uchar  *rowdata = inputGray.ptr<uchar>(i);
			for (int j = 0; j < nc; j++) {
				hist[rowdata[j]]++;			
			}
		}
		//计算概率直方图
		int totalNum = nc;
		float probability[256] = { 0 };
		probability[0] = float(hist[0]) / nc;
		for (int i =1; i < 256; i++) {
			probability[i] = probability[i - 1] + float(hist[i]) / nc;
		}

		//计算百分比的灰度区间
		int minGray;
		int maxGray;
		for (int i = 0;i<256 ; i++) {
			if (probability[i] > percent / 100.0) {
				minGray = i;
				break;
			}
		}
		for (int j = 255; j > 0; j--) {
			if ((1-probability[j]) > percent / 100.0) {
				maxGray = j;
				break;
			}
		}

		//修改图像灰度
		outputGray = inputGray.clone();
		int interval=maxGray-minGray;
		for (int i = 0; i < nr; i++) {
			uchar  *rowdata = outputGray.ptr<uchar>(i);
			for (int j = 0; j < nc; j++) {
				if (rowdata[j] <= minGray) {
					rowdata[j] = 0;
				}
				else if(rowdata[j]>=maxGray)
				{
					rowdata[j] = 255;
				}
				else
				{
					rowdata[j] = static_cast<uchar>((rowdata[j] - minGray) / float(interval) * 255);
				}
			}
		}
	}


	void  clearUpDownBorder(const cv::Mat &binaryPlateImg, cv::Mat &outputPlateImg, int threshold) {
		outputPlateImg = binaryPlateImg.clone();
		for (size_t i = 0; i < outputPlateImg.rows; i++) {
			uchar* inData = outputPlateImg.ptr<uchar>(i);
			int jumpCount = 0;
			for (size_t j = 0; j < outputPlateImg.cols - 1; j++)
			{
				if (inData[j] != inData[j + 1])jumpCount++;
			}
			if (jumpCount < threshold)
			{
				for (size_t j = 0; j < outputPlateImg.cols; j++)inData[j] = 0;
			}
		}
		cv::imshow("before clear", outputPlateImg);


		uchar lastZeroCol = 0;
  		for (size_t i = 0; i < outputPlateImg.cols; i++) {
			uchar whitePointCount=0;
		
		//	uchar stdCharWidth = outputPlateImg.cols / 10;
			for (size_t j = 0; j < outputPlateImg.rows; j++) {
				if (outputPlateImg.at<uchar>(j, i) == 255)whitePointCount++;
			}
			
			if (whitePointCount > 0 && whitePointCount < 4&&lastZeroCol>15) {

				for (size_t j = 0; j < outputPlateImg.rows; j++) {
					outputPlateImg.at<uchar>(j, i) =0;
				}

				lastZeroCol=0;
			}
			else if(whitePointCount==0)
			{
				lastZeroCol=0;
			}
			else
			{
				lastZeroCol++;
			}
			

		}
		cv::imshow("after clear", outputPlateImg);
		cv::imshow("clear Border outputImg", outputPlateImg);
	//	cv::waitKey();
	}


	//float MaxCG:对高频成分的最大增益值,int n：局部半径,int C:对高频的直接增益系数  

	void ACE(cv::Mat &src, int C, int n , float MaxCG ) {

		if (src.channels() == 3) {
			cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);

		}
		int rows = src.rows;

		int cols = src.cols;



		cv::Mat meanLocal; //图像局部均值  

		cv::Mat varLocal;  //图像局部方差  

		cv::Mat meanGlobal;//全局均值

		cv::Mat varGlobal; //全局标准差  
		


		cv::blur(src.clone(), meanLocal, cv::Size(n, n));

		imshow("低通滤波", meanLocal);

		cv::Mat highFreq = src - meanLocal;//高频成分 

		cv::imshow("高频成分", highFreq);



		varLocal = matrixWiseMulti(highFreq, highFreq);

		cv::blur(varLocal, varLocal, cv::Size(n, n));

		//换算成局部标准差  

		varLocal.convertTo(varLocal, CV_32F);

		for (int i = 0; i < rows; i++) {

			for (int j = 0; j < cols; j++) {

				varLocal.at<float>(i, j) = (float)sqrt(varLocal.at<float>(i, j));

			}

		}

		cv::meanStdDev(src, meanGlobal, varGlobal);

		cv::Mat gainArr = 0.5 * meanGlobal / varLocal;//增益系数矩阵  



		//对增益矩阵进行截止  

		for (int i = 0; i < rows; i++) {

			for (int j = 0; j < cols; j++) {

				if (gainArr.at<float>(i, j) > MaxCG) {

					gainArr.at<float>(i, j) = MaxCG;

				}

			}

		}

		gainArr.convertTo(gainArr, CV_8U);

		gainArr = matrixWiseMulti(gainArr, highFreq);

		cv::Mat dst1 = meanLocal + gainArr;

		cv::imshow("变增益方法", dst1);
		
		cv::Mat dst2 = meanLocal + C * highFreq;

		cv::imshow("恒增益方法", dst2);
		src = dst1;
	//	cv::waitKey(0);

	}


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

		int s = src.cols >> 4;

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
		dst = src.clone();
		if (dst.type() == CV_8UC3) {
			cvtColor(dst, dst, CV_BGR2GRAY);
		}

		int x1, y1, x2, y2;
		int count = 0;
		long long sum = 0;
		int S = 9;  //划分区域的大小S*S,S越大，边缘越粗
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
	void getRotatedRectArea(const cv::Mat &srcImg, cv::RotatedRect &rRect, cv::Mat &outImg,cv::Mat &transMat, bool extend) {

		float widthExtend = 1;
		float hightExtend = 1;
		cv::RotatedRect &rRectCopy = rRect;
		float ratio = float(rRectCopy.size.width) / rRectCopy.size.height;
		if (extend) {
			widthExtend = 1.6;
			hightExtend = 1.2;
			//根据旋转矩形的长宽比进行扩大		
			if (ratio < 1) {
 				ratio = 1 / ratio;
			//	if (ratio < 3.4)rRectCopy.size.height = rRectCopy.size.height *(8.4 / ratio - 1);//原为7.8
				rRectCopy.size.height = rRectCopy.size.height *1.1;
				if (rRectCopy.angle < -10 || rRectCopy.angle>-80) {
					rRectCopy.size.width += 4;
				}
				else
				{
					rRectCopy.size.width += 2;
				}
			}
			else
			{
			//	if (ratio < 3.4)rRectCopy.size.width = rRectCopy.size.width *(8.4 / ratio - 1);
				rRectCopy.size.width = rRectCopy.size.width *1.1;
				if (rRectCopy.angle<-10 || rRectCopy.angle>-80) {
					 rRectCopy.size.height += 4;
				}
				else
				{
					rRectCopy.size.height += 2;
				}
			}
		}
		cv::Point2f vertices[4];
		rRectCopy.points(vertices);
		if (rRectCopy.size.width < rRectCopy.size.height) {
			std::swap(rRectCopy.size.width, rRectCopy.size.height);
			rRectCopy.angle = rRectCopy.angle + 90;
			std::swap(vertices[0], vertices[3]);
			std::swap(vertices[0], vertices[2]);
			std::swap(vertices[0], vertices[1]);
		}

		cv::Point2f dstPoint[3];
	//	dstPoint[0] = cv::Point2f(0, rRectCopy.size.height);
		dstPoint[0] = cv::Point2f(0,36);
		dstPoint[1] = cv::Point2f(0, 0);
	//	dstPoint[2] = cv::Point2f(rRectCopy.size.width, 0);
		dstPoint[2] = cv::Point2f(136, 0);
		transMat = cv::getAffineTransform(vertices, dstPoint);
		//cv::GaussianBlur(src)//此处可考虑使用高斯模糊
	//	cv::warpAffine(srcImg, outImg, transMat, cv::Size(rRectCopy.size.width, rRectCopy.size.height));
		cv::warpAffine(srcImg, outImg, transMat, cv::Size(136,36));
	}

	//画栅格线
	void drawGrid(const cv::Mat &srcImg, cv::Mat &outputImg, int gridx, int gridy) {

		outputImg = srcImg.clone();
		for (int i = 1; i < gridy; i++) {
			cv::line(outputImg, cv::Point(0, outputImg.size().height*i / gridy), cv::Point(IDEA_WIDTH, outputImg.size().height*i / gridy), cv::Scalar(255, 255, 255), 3);
		}
		for (int i = 1; i < gridx; i++) {
			cv::line(outputImg, cv::Point(IDEA_WIDTH * i / gridx, 0), cv::Point(IDEA_WIDTH * i / gridx, outputImg.size().height), cv::Scalar(255, 255, 255), 3);
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
			mu += i * (double)h[i];//

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


