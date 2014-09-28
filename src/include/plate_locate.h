//////////////////////////////////////////////////////////////////////////
// Name:	    plate_locate Header
// Version:		1.0
// Date:	    2014-09-19
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:  
// Defines CPlateLocate
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_LOCATE_H__
#define __PLATE_LOCATE_H__

#include "prep.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateLocate 
{
public:
	CPlateLocate();

	//! 车牌定位
	int plateLocate(Mat, vector<Mat>& );

	//! 车牌的尺寸验证
	bool verifySizes(RotatedRect mr);

	//! 结果车牌显示
	Mat showResultMat(Mat src, Size rect_size, Point2f center);

	//! PlateDection所用常量
	static const int GAUSSIANBLUR_SIZE = 5;
	static const int SOBEL_SCALE = 1;
	static const int SOBEL_DELTA = 0;
	static const int SOBEL_DDEPTH = CV_16S;
	static const int SOBEL_X_WEIGHT = 1;
	static const int SOBEL_Y_WEIGHT = 0;
	static const int MORPH_SIZE_WIDTH = 17;
	static const int MORPH_SIZE_HEIGHT = 3;

	//! showResultMat所用常量
	static const int WIDTH = 136;
	static const int HEIGHT = 36;
	static const int TYPE = CV_8UC3;
};

}	/*! \namespace easypr*/

#endif /* endif __PLATE_LOCATE_H__ */