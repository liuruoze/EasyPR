#include "easypr/core/plate_judge.h"
#include "easypr/config.h"

namespace easypr {

PlateJudge* PlateJudge::instance_ = nullptr;

PlateJudge* PlateJudge::instance() {
  if (!instance_) {
    instance_ = new PlateJudge;
  }
  return instance_;
}

PlateJudge::PlateJudge() { svm_ = ml::SVM::load<ml::SVM>(kDefaultSvmPath); }

//! 对单幅图像进行SVM判断

int PlateJudge::plateJudge(const Mat &inMat, int &result) {
  Mat features;
  getHistogramFeatures(inMat, features);

  float response = svm_->predict(features);
  result = (int) response;

  return 0;
}

//! 对多幅图像进行SVM判断

int PlateJudge::plateJudge(const std::vector<Mat> &inVec,
                           std::vector<Mat> &resultVec) {
  int num = inVec.size();
  for (int j = 0; j < num; j++) {
    Mat inMat = inVec[j];

    int response = -1;
    plateJudge(inMat, response);

    if (response == 1) resultVec.push_back(inMat);
  }
  return 0;
}

//! 对多幅车牌进行SVM判断

int PlateJudge::plateJudge(const std::vector<CPlate> &inVec,
                           std::vector<CPlate> &resultVec) {
  int num = inVec.size();
  for (int j = 0; j < num; j++) {
    CPlate inPlate = inVec[j];
    Mat inMat = inPlate.getPlateMat();

    int response = -1;
    plateJudge(inMat, response);

    if (response == 1)
      resultVec.push_back(inPlate);
    else {
      int w = inMat.cols;
      int h = inMat.rows;

      //再取中间部分判断一次

      Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
      Mat tmpDes = inMat.clone();
      resize(tmpmat, tmpDes, Size(inMat.size()));

      plateJudge(tmpDes, response);

      if (response == 1) resultVec.push_back(inPlate);
    }
  }
  return 0;
}
}
