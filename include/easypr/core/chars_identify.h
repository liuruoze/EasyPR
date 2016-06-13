#ifndef EASYPR_CORE_CHARSIDENTIFY_H_
#define EASYPR_CORE_CHARSIDENTIFY_H_

#include "easypr/util/kv.h"
#include <memory>
#include <opencv2/opencv.hpp>

namespace easypr {

class CharsIdentify {
 public:
  static CharsIdentify* instance();

  std::pair<std::string, std::string> identify(cv::Mat input);

 private:
  CharsIdentify();

  static CharsIdentify* instance_;
  cv::Ptr<cv::ml::ANN_MLP> ann_;
  std::shared_ptr<Kv> kv_;
};
}

#endif  //  EASYPR_CORE_CHARSIDENTIFY_H_