#pragma once
#include <QObject>
#include"PlateLocate.h"

class Controler : public QObject
{
	Q_OBJECT

private:	
	
	class CGarbo {//此类的唯一工作：在析构函数中删除Controler单例
		~CGarbo() {
			if (controler) { 
				delete controler;
				controler = nullptr;
			}
		}
	};

	static Controler* controler;//单例
	
	std::vector<std::string> vecImgPath;/**< 图片文件列表 */
	int currImgCount;/**< 当前图片索引 */
	int imgTotalCount;/**< 总的图片数目 */


public:
	double totaltime = 0;
	double alltime = 0;
	Controler(QObject *parent = Q_NULLPTR);
	~Controler();
	static Controler* getControler(QObject *parent = Q_NULLPTR);
	static bool destroy();
	static CGarbo Garbo;
	void showImg(const cv::Mat &img);
	QImage plateImg;
	PlateLocate *PR;


	inline bool setCurrImgCount(int Count) { currImgCount = Count;  emit signalShowCurrCount(currImgCount); return true; }
	inline int getCurrImgCount(){return currImgCount; }
	inline bool setImgTotalCount(int totalCount) { imgTotalCount = totalCount; emit signalShowTotalCount(imgTotalCount); return true; }
	inline int getImgTotalCount() { return imgTotalCount; }
	//inline QString getimgDirectory() { return imgDirectory; }
	inline void ShowMessage(const std::string &message, const std::string &title = "") { 
		     emit signalShowMessage(QString::fromStdString(message), QString::fromStdString(title)); };
	

//发送给界面的信号	
public:signals:
	void signalShowImg(const QImage &img);
	void signalShowMessage( const QString &message, const QString &title="");
	void signalShowTotalCount(int totalCount);
	void signalShowCurrCount( int currCount);

//接收界面的消息并行动
private slots:
	void slotReadImgList(QString Directory);
	void slotReadImgList(const QStringList &imgPathList);
	void slotProcessImg();
	void slotBatchTest();
	void slotStartTrain(const QString &posiPath, const QString &negaPath, const QString &saveModelPath);
	void slotModelTest(const QString &testSamplePath);
	
};


