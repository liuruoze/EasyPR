//////////////////////////////////////////////////////////////////////////
// Name:	    plate_judge Header
// Version:		1.0
// Date:	    2014-09-22
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:  
// Defines CPlateLocate
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_JUDGE_H__
#define __PLATE_JUDGE_H__

#include "prep.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateJudge 
{
public:
	CPlateJudge();

	//! 车牌判断
	int plateJudge(const vector<Mat>&, vector<Mat>&);

	//! 直方图均衡
	Mat histeq(Mat);

	//! 装载SVM模型
	void LoadModel();

	//! 装载SVM模型
	void LoadModel(string s);

	//! 设置与读取模型路径
	inline void setModelPath(string path){	m_path = path;	}
	inline string getModelPath() const{	 return m_path;	}

private:
	//！使用的SVM模型
	CvSVM svm;

	//! 模型存储路径
	string m_path;
};

}	/*! \namespace easypr*/

#endif /* endif __PLATE_JUDGE_H__ */