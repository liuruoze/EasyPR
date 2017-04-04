#ifndef EASYPR_TRAIN_ANNCHTRAIN_H_
#define EASYPR_TRAIN_ANNCHTRAIN_H_

#include "easypr/train/train.h"
#include "easypr/util/kv.h"
#include "easypr/core/feature.h"
#include <memory>

namespace easypr {

class AnnChTrain : public ITrain {
 public:
  explicit AnnChTrain(const char* chars_folder, const char* xml);

  virtual void train();

  virtual void test();

  std::pair<std::string, std::string> identifyChinese(cv::Mat input);
  cv::Mat AnnChTrain::generateGraySyntheticImage(const cv::Mat& image);

 private:
  virtual cv::Ptr<cv::ml::TrainData> tdata();

  void AnnChTrain::trainVal(size_t number_for_count = 100);

  cv::Ptr<cv::ml::ANN_MLP> ann_;
  const char* ann_xml_;
  const char* chars_folder_;

  std::shared_ptr<Kv> kv_;
  int type;

  annCallback extractFeature;
};
}

#endif  // EASYPR_TRAIN_ANNCHTRAIN_H_