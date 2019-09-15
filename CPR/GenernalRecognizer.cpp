#include "stdafx.h"
#include "GenernalRecognizer.h"
#include "PlateLocate.h"
#include"CPlate.h"
#include"util.h"

GenernalRecognizer::GenernalRecognizer()
{
}


GenernalRecognizer::~GenernalRecognizer()
{
}
void GenernalRecognizer::SegmentBasedSequenceRecognition(std::vector<CPlate> &vecPlate) {
	
	for (int i = 0; i < vecPlate.size(); i++) {
		CPlate &plate = vecPlate[i];

		std::string characters;
		cv::Mat	affinedImg;
		affinedImg = plate.plateBinaryImg.clone();
		if (affinedImg.channels() == 3) {
			cv::cvtColor(affinedImg, affinedImg, CV_BGR2GRAY);
		}
		for (int i = 0; i < plate.characterRects.size(); i++) {
			cv::RotatedRect rRect = plate.characterRects[i];
			cv::Rect2f characterRect;
			util::calcSafeRect(rRect, affinedImg.size(), characterRect);
			cv::Mat tempImg = cv::Mat(affinedImg, characterRect);
			cv::imshow("semgIMG", tempImg);
			//cv::waitKey(0);
			if (i == 0) {
				characters += recognizeCharacter(tempImg, CHINESE);
			}
			else if (i == 1) {
				characters += recognizeCharacter(tempImg, LETTER);
			}
			else if (i == 2) {
				characters += recognizeCharacter(tempImg, LETTER_NUM);
			}
			else if (i == 7) {

			}
			else
			{
				characters += recognizeCharacter(tempImg, LETTER_NUM);
			}

			//character = util::GBKToUTF8(character.data());
			qDebug() << QString::fromStdString(characters);
		}
		plate.setPlateChars(characters);


	}
}