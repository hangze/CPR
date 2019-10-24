#include "stdafx.h"
#include "CPlate.h"


 cv::RotatedRect CPlate::getRotatedRect() {
	cv::RotatedRect tempRRect = plateRotaRect;
//	float ratio = float(IDEA_WIDTH) / srcSize.width;
	float ratio = 1.0;

	tempRRect.center.x = tempRRect.center.x*ratio;
	tempRRect.center.y = tempRRect.center.y * ratio;
	tempRRect.size.width = tempRRect.size.width*ratio;
	tempRRect.size.height = tempRRect.size.height*ratio;
	return tempRRect;
}

 void CPlate::setPlateRotaRect(cv::RotatedRect rr) {
	plateRotaRect = rr;
	float ratio = srcSize.width / float(IDEA_WIDTH);
	plateRotaRect.center.x = plateRotaRect.center.x*ratio;
	plateRotaRect.center.y = plateRotaRect.center.y * ratio;
	plateRotaRect.size.width = plateRotaRect.size.width*ratio;
	plateRotaRect.size.height = plateRotaRect.size.height*ratio;

}

CPlate::CPlate()
{
}


CPlate::~CPlate()
{
}
