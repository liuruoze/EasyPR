#ifndef EASYPR_TRAIN_ANNTRAIN_H_
#define EASYPR_TRAIN_ANNTRAIN_H_

#include <opencv2/opencv.hpp>

namespace easypr{

  class AnnTrain{
  public:
    explicit AnnTrain(const char* chars_folder, const char* xml);

    void train(const int & neurons = 40);

    void test();

  private:
    cv::Ptr<cv::ml::TrainData> train_data();
    cv::Ptr<cv::ml::ANN_MLP> ann_;

    const char* ann_xml_;
    const char* chars_folder_;
  };

}

#endif // EASYPR_TRAIN_ANNTRAIN_H_