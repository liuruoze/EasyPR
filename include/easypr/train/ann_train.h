#ifndef EASYPR_TRAIN_ANNTRAIN_H_
#define EASYPR_TRAIN_ANNTRAIN_H_

#include "easypr/train/train.h"
#include "easypr/util/kv.h"
#include <memory>

namespace easypr {

class AnnTrain : public ITrain {
 public:
  explicit AnnTrain(const char* chars_folder, const char* xml);

  virtual void train();

  virtual void test();

  std::pair<std::string, std::string> identifyChinese(cv::Mat input);
  std::pair<std::string, std::string> identify(cv::Mat input);

 private:
  virtual cv::Ptr<cv::ml::TrainData> tdata();

  cv::Ptr<cv::ml::TrainData> sdata(size_t number_for_count = 100);

  cv::Ptr<cv::ml::ANN_MLP> ann_;
  const char* ann_xml_;
  const char* chars_folder_;

  std::shared_ptr<Kv> kv_;
  int type;
};
}

#endif  // EASYPR_TRAIN_ANNTRAIN_H_