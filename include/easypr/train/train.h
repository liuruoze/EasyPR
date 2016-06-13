#ifndef EASYPR_TRAIN_TRAIN_H_
#define EASYPR_TRAIN_TRAIN_H_

#include <opencv2/opencv.hpp>

namespace easypr {

class ITrain {
 public:
  ITrain();

  virtual ~ITrain();

  virtual void train() = 0;

  virtual void test() = 0;

 private:
  virtual cv::Ptr<cv::ml::TrainData> tdata() = 0;
};
}

#endif  // EASYPR_TRAIN_TRAIN_H_