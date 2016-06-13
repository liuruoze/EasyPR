#ifndef EASYPR_TRAIN_ANNTRAIN_H_
#define EASYPR_TRAIN_ANNTRAIN_H_

#include "easypr/train/train.h"

namespace easypr {

class AnnTrain : public ITrain {
 public:
  explicit AnnTrain(const char* chars_folder, const char* xml);

  virtual void train();

  virtual void test();

 private:
  virtual cv::Ptr<cv::ml::TrainData> tdata();

  cv::Ptr<cv::ml::ANN_MLP> ann_;
  const char* ann_xml_;
  const char* chars_folder_;
};
}

#endif  // EASYPR_TRAIN_ANNTRAIN_H_