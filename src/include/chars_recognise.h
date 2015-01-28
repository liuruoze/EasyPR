//////////////////////////////////////////////////////////////////////////
// Name:	    chars_recognise Header
// Version:		1.0
// Date:	    2014-09-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsRecognise
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_RECOGNISE_H__
#define __CHARS_RECOGNISE_H__

#include "prep.h"

#include "chars_segment.h"
#include "chars_identify.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CCharsRecognise 
{
public:
	CCharsRecognise();

	//! 字符分割与识别
	int charsRecognise(Mat, String&);

	//! 装载ANN模型
	void LoadANN(string s);

	//! 是否开启调试模式
	inline void setCRDebug(int param){ m_charsSegment->setDebug(param);}

	//! 获取调试模式状态
	inline int getCRDebug(){ return m_charsSegment->getDebug();}


	//! 获得车牌颜色
	inline string getPlateType(Mat input) const
	{
		string color = "未知";
		int result = m_charsSegment->getPlateType(input);
		if (1 == result)
			color = "蓝牌";
		if (2 == result)
			color = "黄牌";
		return color;
	}

	//! 设置变量
	inline void setLiuDingSize(int param){ m_charsSegment->setLiuDingSize(param);}
	inline void setColorThreshold(int param){ m_charsSegment->setColorThreshold(param);}
	inline void setBluePercent(float param){ m_charsSegment->setBluePercent(param);}
	inline float getBluePercent() const { return m_charsSegment->getBluePercent();}
	inline void setWhitePercent(float param){ m_charsSegment->setWhitePercent(param);}
	inline float getWhitePercent() const { return m_charsSegment->getWhitePercent();}

private:
	//！字符分割
	CCharsSegment* m_charsSegment;

	//! 字符识别
	CCharsIdentify* m_charsIdentify;
};

}	/* \namespace easypr  */

#endif /* endif __CHARS_RECOGNISE_H__ */