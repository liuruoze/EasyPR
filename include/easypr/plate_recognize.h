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

#include "easypr/plate_detect.h"
#include "easypr/chars_recognise.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateRecognize : public CPlateDetect, public CCharsRecognise {
 public:
  CPlateRecognize();

  //! 车牌检测与字符识别
  int plateRecognize(cv::Mat src, std::vector<std::string>& licenseVec,
                     int index = 0);

  //! 生活模式与工业模式切换
  inline void setLifemode(bool param) { CPlateDetect::setPDLifemode(param); }

  //! 是否开启调试模式
  inline void setDebug(bool param) {
    CPlateDetect::setPDDebug(param);
    CCharsRecognise::setCRDebug(param);
  }
};

} /* \namespace easypr  */

#endif /* endif __PLATE_RECOGNITION_H__ */