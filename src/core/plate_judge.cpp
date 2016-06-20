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

  PlateJudge::PlateJudge() { 
    svm_ = ml::SVM::load<ml::SVM>(kDefaultSvmPath); 
    extractFeature = getHistogramFeatures;
  }

  void PlateJudge::LoadModel(std::string path) {
    svm_->clear();
    svm_->ml::SVM::load<ml::SVM>(path);
  }

  //! 对单幅图像进行SVM判断

  int PlateJudge::plateJudge(const Mat &inMat, int &result) {
    Mat features;
    extractFeature(inMat, features);

    float response = svm_->predict(features);
    result = (int)response;

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

  //! 设置车牌图像的置信度
  //! 返回值，0代表是车牌，其他值代表不是
  int PlateJudge::plateSetScore(CPlate& plate) {
    Mat features;
    extractFeature(plate.getPlateMat(), features);

    float score = svm_->predict(features, noArray(), cv::ml::StatModel::Flags::RAW_OUTPUT);

    // score值代表离margin的距离，小于0代表是车牌，大于0代表不是车牌
    // 当小于0时，值越小代表是车牌的概率越大
    plate.setPlateScore(score);

    if (score < 0)
      return 0;
    else
      return -1;
  }

  ////! 比较函数
  //struct PlateScoreCompaer {
  //  bool operator() (const CPlate& i, const CPlate& j) { return (i.getPlateScore() < j.getPlateScore()); }
  //} plateScoreCompaerObject;


  //! 非极大值抑制
  void NMS(std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, double overlap) {

    std::sort(inVec.begin(), inVec.end());

    std::vector<CPlate>::iterator it = inVec.begin();
    for (; it != inVec.end(); ++it) {
      CPlate plateSrc = *it;
      //std::cout << "plateScore:" << plateSrc.getPlateScore() << std::endl;
      Rect rectSrc = plateSrc.getPlatePos().boundingRect();

      std::vector<CPlate>::iterator itc = it + 1;

      for (; itc != inVec.end();) {
        CPlate plateComp = *itc;
        Rect rectComp = plateComp.getPlatePos().boundingRect();
        Rect rectInter = rectSrc & rectComp;
        Rect rectUnion = rectSrc | rectComp;
        double r = double(rectInter.area()) / double(rectUnion.area());
        if (r > overlap) {
          itc = inVec.erase(itc);
        }
        else {
          ++itc;
        }
      }
    }

    resultVec = inVec;
  }

  //! 使用非极大值抑制的车牌判断
  int PlateJudge::plateJudgeUsingNMS(const std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, int maxPlates) {
    std::vector<CPlate> plateVec;
    int num = inVec.size();

    for (int j = 0; j < num; j++) {
      CPlate plate = inVec[j];
      Mat inMat = plate.getPlateMat();

      if (0) {
        imshow("inMat", inMat);
        waitKey(0);
      }

      int result = plateSetScore(plate);
      if (result == 0) {
        plateVec.push_back(plate);
        if (0) {
          imshow("plateMat", inMat);
          waitKey(0);
        }
      }
      else {
        int w = inMat.cols;
        int h = inMat.rows;

        //再取中间部分判断一次

        Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
        Mat tmpDes = inMat.clone();
        resize(tmpmat, tmpDes, Size(inMat.size()));

        plate.setPlateMat(tmpDes);

        int resultCascade = plateSetScore(plate);

        if (resultCascade == 0) {
          plateVec.push_back(plate);
        }
      }
    }

    std::vector<CPlate> reDupPlateVec;

    // 使用非极大值抑制来去除那些重叠的车牌
    // overlap阈值设置为0.5
    double overlap = 0.5;
    NMS(plateVec, reDupPlateVec, overlap);
  
    std::vector<CPlate>::iterator it = reDupPlateVec.begin();
    int count = 0;
    for (; it != reDupPlateVec.end(); ++it) {
      resultVec.push_back(*it);
      count++;
      if (count >= maxPlates)
        break;
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
