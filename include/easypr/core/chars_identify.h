//////////////////////////////////////////////////////////////////////////
// Name:	    chars_identify Header
// Version:		1.0
// Date:	    2014-09-25
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsIdentify
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
    */
namespace easypr {

  class CCharsIdentify {
  public:
    CCharsIdentify();

    //! 字符分割
    string charsIdentify(Mat input, bool isChinese, bool isSpeci);

  private:
    //! 字符分类
    int classify(Mat f, bool isChinses, bool isSpeci);

    //！使用的ANN模型
    cv::Ptr<ml::ANN_MLP> ann_;
  };

} /* \namespace easypr  */

#endif /* endif __CHARS_IDENTIFY_H__ */