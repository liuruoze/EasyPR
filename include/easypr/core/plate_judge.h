#ifndef EASYPR_CORE_PLATEJUDGE_H_
#define EASYPR_CORE_PLATEJUDGE_H_

#include "easypr/core/plate.hpp"
#include "easypr/core/feature.h"

namespace easypr {

class PlateJudge {
 public:
  static PlateJudge* instance();
  void LoadModel(std::string path);

  int plateJudgeUsingNMS(const std::vector<CPlate>&, std::vector<CPlate>&, int maxPlates = 5);
  int plateSetScore(CPlate& plate);

  int plateJudge(const Mat& plateMat);
  int plateJudge(const std::vector<Mat> &inVec,
    std::vector<Mat> &resultVec);
  int plateJudge(const std::vector<CPlate> &inVec,
    std::vector<CPlate> &resultVec);

 private:
  // singleton
  PlateJudge();

  static PlateJudge* instance_;

  svmCallback extractFeature;

  cv::Ptr<ml::SVM> svm_;

};
}

#endif  // EASYPR_CORE_PLATEJUDGE_H_