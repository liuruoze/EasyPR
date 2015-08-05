#ifndef EASYPR_TRAIN_ANNTRAIN_H_
#define EASYPR_TRAIN_ANNTRAIN_H_

#include <opencv2/opencv.hpp>
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"

namespace easypr{

  class AnnTrain{
  public:
    explicit AnnTrain(const char* chars_folder, const char* zhchars_folder, const char* xml);

    void train(const int & neurons = 40);

  private:
    void getTrainData();

    cv::Ptr<cv::ml::ANN_MLP> ann_;
    cv::Ptr<cv::ml::TrainData> train_data_;

    const char* ann_xml_;
    const char* chars_folder_;
    const char* zhchars_folder_;
  };

}

#endif // EASYPR_TRAIN_ANNTRAIN_H_