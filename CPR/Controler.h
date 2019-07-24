#pragma once
#include <QObject>
#include"PlateLocate.h"




class Controler : public QObject
{
	Q_OBJECT

public:
	Controler(QObject *parent = Q_NULLPTR);
	~Controler();
	
	inline bool setCurrImgCount(int Count) { currImgCount = Count; return true; }
	inline int getCurrImgCount(){return currImgCount; }
	inline bool setImgTotalCount(int totalCount) { imgTotalCount = totalCount; return true; }
	inline int getImgTotalCount() { return imgTotalCount; }
	inline QString* getimgDirectory() { return imgDirectory; }

	static void imgReadFailure();
	static void showImg();

private:
	QStringList imgNameList;/**< 图片文件列表 */
	int currImgCount;/**< 当前图片索引 */
	int imgTotalCount;/**< 总的图片数目 */
	QString *imgDirectory = new QString;/**< 文件夹路径字符串地址 */
	CPlate currPlate;
	//std::vector<CPlate> Plates;
	
private slots:
	void slotReadImgList(QString *Directory);
	void slotProcessImg();
	void slotBatchTestBtn();
};



