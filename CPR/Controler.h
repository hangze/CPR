#pragma once
#include <QObject>
#include"PlateLocate.h"
class CPlate;

class Controler : public QObject
{
	Q_OBJECT

private:	
	
	class CGarbo {//类唯一工作：在析构函数中删除Controler单例
		~CGarbo() {
			if (controler) { 
				delete controler;
				controler = nullptr;
			}
		}
	};

	static Controler* controler;//单例
	
	QStringList imgNameList;/**< 图片文件列表 */
	int currImgCount;/**< 当前图片索引 */
	int imgTotalCount;/**< 总的图片数目 */
	QString *imgDirectory = new QString;/**< 文件夹路径字符串地址 */
	CPlate currPlates;
	//std::vector<CPlate> Plates;

public:
	Controler(QObject *parent = Q_NULLPTR);
	~Controler();
	static Controler* getControler(QObject *parent = Q_NULLPTR);
	static bool destroy();
	static CGarbo Garbo;
	void showImg(const cv::Mat &img);
	QImage tempImg2;
	//cv::Mat tempImg;

	inline bool setCurrImgCount(int Count) { currImgCount = Count; return true; }
	inline int getCurrImgCount(){return currImgCount; }
	inline bool setImgTotalCount(int totalCount) { imgTotalCount = totalCount; return true; }
	inline int getImgTotalCount() { return imgTotalCount; }
	inline QString* getimgDirectory() { return imgDirectory; }
	inline void ShowMessage(const std::string &message, const std::string &title = "") { 
		     emit signalShowMessage(QString::fromStdString(message), QString::fromStdString(title)); };
	
	//static void imgReadFailure() {ShowMessage };
	
public:signals:
	void signalShowImg(const QImage &img);
	void signalShowMessage( const QString &message, const QString &title="");
	
private slots:
	void slotReadImgList(QString *Directory);
	void slotProcessImg();
	void slotBatchTest();
	void slotStartTrain(const QString &posiPath, const QString &negaPath, const QString &saveModelPath);
	void slotModelTest(const QString &testSamplePath);
};


