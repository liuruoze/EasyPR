#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>

namespace easypr {

class CharsIdentify {
 public:
    static CharsIdentify* instance();

    std::pair<std::string, std::string> identify(cv::Mat input);

 private:
    CharsIdentify();

    static CharsIdentify* instance_;
    //！使用的ANN模型
    cv::Ptr<cv::ml::ANN_MLP> ann_;
};

}

#endif