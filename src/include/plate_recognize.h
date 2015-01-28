//////////////////////////////////////////////////////////////////////////
// Name:	    plate_recognize Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CPlateRecognize
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_RECOGNIZE_H__
#define __PLATE_RECOGNIZE_H__

#include "prep.h"

#include "plate_detect.h"
#include "chars_recognise.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateRecognize : public CPlateDetect, public CCharsRecognise
{
public:
	CPlateRecognize();

	//! 车牌检测与字符识别
	int plateRecognize(Mat, vector<string>&);

	//! 生活模式与工业模式切换
	inline void setLifemode(bool param)
	{
		CPlateDetect::setPDLifemode(param);
	}

	//! 是否开启调试模式
	inline void setDebug(int param)
	{ 
		CPlateDetect::setPDDebug(param);
		CCharsRecognise::setCRDebug(param);
	}

	////! 车牌定位与判断
	//int plateDetect(Mat, vector<Mat>&);

	////! 字符分割与识别
	//int charsRecognise(Mat, String&);

	////! 装载SVM
	//void LoadSVM(string s);

	////! 装载ANN模型
	//void LoadANN(string s);

private:
	////！车牌检测
	//CPlateDetect* m_plateDetect;

	////! 字符识别
	//CCharsRecognise* m_charsRecognise;
};

}	/* \namespace easypr  */

#endif /* endif __PLATE_RECOGNITION_H__ */