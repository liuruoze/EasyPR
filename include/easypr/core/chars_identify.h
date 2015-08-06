#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>
#include "easypr/util/util.h"

using namespace cv;
using namespace std;

namespace easypr {

  class CharsIdentify {
  public:
    static CharsIdentify* instance();

    string identify(Mat input, bool isChinese, bool isSpeci);

  private:
    CharsIdentify();

    static CharsIdentify* instance_;

    //! 字符分类
    int classify(Mat f, bool isChinses, bool isSpeci);

    //！使用的ANN模型
    cv::Ptr<cv::ml::ANN_MLP> ann_;
  };

}

#endif