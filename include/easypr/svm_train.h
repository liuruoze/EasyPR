//
// Created by Micooz on 15/5/21.
//

#ifndef EASYPR_SVM_TRAIN_H
#define EASYPR_SVM_TRAIN_H

#include <vector>
#include <opencv2/opencv.hpp>
#include "easypr/feature.h"

namespace easypr {

class Svm {
 public:

  int train(bool dividePrepared = true, bool trainPrepared = true,
            svmCallback getFeatures = getHistogramFeatures);

  void accuracy(cv::Mat& testingclasses_preditc, cv::Mat& testingclasses_real);

  void getNoPlateTest(std::vector<cv::Mat>& testingImages,
                      std::vector<int>& testingLabels);

  void getHasPlateTest(std::vector<cv::Mat>& testingImages,
                       std::vector<int>& testingLabels);

  void getNoPlateTrain(cv::Mat& trainingImages,
                       std::vector<int>& trainingLabels,
                       svmCallback getFeatures = getHisteqFeatures);

  void getHasPlateTrain(cv::Mat& trainingImages,
                        std::vector<int>& trainingLabels,
                        svmCallback getFeatures = getHisteqFeatures);

  void learn2NoPlate(float bound = 0.7);

  void learn2HasPlate(float bound = 0.7);

};


}


#endif //EASYPR_SVM_TRAIN_H
