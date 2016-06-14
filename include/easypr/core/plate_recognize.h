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
#ifndef EASYPR_CORE_PLATERECOGNIZE_H_
#define EASYPR_CORE_PLATERECOGNIZE_H_

#include "easypr/core/plate_detect.h"
#include "easypr/core/chars_recognise.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateRecognize : public CPlateDetect, public CCharsRecognise {
 public:
  CPlateRecognize();

  //! 车牌检测与字符识别
  int plateRecognize(Mat src, std::vector<CPlate> &licenseVec);
  int plateRecognize(Mat src, std::vector<std::string> &licenseVec);

  int plateRecognizeAsText(Mat src, std::vector<CPlate> &licenseVec);
  int plateRecognizeAsTextNM(Mat src, std::vector<CPlate> &licenseVec);

  //! 生活模式与工业模式切�

  inline void setLifemode(bool param) { CPlateDetect::setPDLifemode(param); }

  //! 是否开启调试模�

  inline void setDebug(bool param) { CPlateDetect::setPDDebug(param); }
};

} /* \namespace easypr  */

#endif  // EASYPR_CORE_PLATERECOGNIZE_H_