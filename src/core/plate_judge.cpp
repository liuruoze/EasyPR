#include "easypr/core/plate_judge.h"
#include "easypr/config.h"
#include "easypr/core/core_func.h"
#include "easypr/core/params.h"

namespace easypr {

  PlateJudge* PlateJudge::instance_ = nullptr;

  PlateJudge* PlateJudge::instance() {
    if (!instance_) {
      instance_ = new PlateJudge;
    }
    return instance_;
  }

  PlateJudge::PlateJudge() { 
    bool useLBP = false;
    if (useLBP) {
      LOAD_SVM_MODEL(svm_, kLBPSvmPath);
      extractFeature = getLBPFeatures;
    }
    else {
      LOAD_SVM_MODEL(svm_, kHistSvmPath);
      extractFeature = getHistomPlusColoFeatures;
    }
  }

  void PlateJudge::LoadModel(std::string path) {
    if (path != std::string(kDefaultSvmPath)) {
      if (!svm_->empty())
        svm_->clear();
      LOAD_SVM_MODEL(svm_, path);
    }
  }

  // set the score of plate
  // 0 is plate, -1 is not.
  int PlateJudge::plateSetScore(CPlate& plate) {
    Mat features;
    extractFeature(plate.getPlateMat(), features);
    float score = svm_->predict(features, noArray(), cv::ml::StatModel::Flags::RAW_OUTPUT);
    //std::cout << "score:" << score << std::endl;
    if (0) {
      imshow("plate", plate.getPlateMat());
      waitKey(0);
      destroyWindow("plate");
    }
    // score is the distance of margin，below zero is plate, up is not
    // when score is below zero, the samll the value, the more possibliy to be a plate.
    plate.setPlateScore(score);
    if (score < 0.5) return 0;
    else return -1;      
  }

  int PlateJudge::plateJudge(const Mat& plateMat) {
    CPlate plate;
    plate.setPlateMat(plateMat);
    return plateSetScore(plate);
  }

  int PlateJudge::plateJudge(const std::vector<Mat> &inVec,
    std::vector<Mat> &resultVec) {
    int num = inVec.size();
    for (int j = 0; j < num; j++) {
      Mat inMat = inVec[j];

      int response = -1;
      response = plateJudge(inMat);

      if (response == 0) resultVec.push_back(inMat);
    }
    return 0;
  }

  int PlateJudge::plateJudge(const std::vector<CPlate> &inVec,
    std::vector<CPlate> &resultVec) {
    int num = inVec.size();
    for (int j = 0; j < num; j++) {
      CPlate inPlate = inVec[j];
      Mat inMat = inPlate.getPlateMat();
      int response = -1;
      response = plateJudge(inMat);

      if (response == 0)
        resultVec.push_back(inPlate);
      else {
        int w = inMat.cols;
        int h = inMat.rows;
        Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
        Mat tmpDes = inMat.clone();
        resize(tmpmat, tmpDes, Size(inMat.size()));

        response = plateJudge(tmpDes);
        if (response == 0) resultVec.push_back(inPlate);
      }
    }
    return 0;
  }

  // non-maximum suppression
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
        float iou = computeIOU(rectSrc, rectComp);
        if (iou > overlap) {
          itc = inVec.erase(itc);
        }
        else {
          ++itc;
        }
      }
    }
    resultVec = inVec;
  }

  // judge plate using nms
  int PlateJudge::plateJudgeUsingNMS(const std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, int maxPlates) {
    std::vector<CPlate> plateVec;
    int num = inVec.size();
    bool useCascadeJudge = true;

    for (int j = 0; j < num; j++) {
      CPlate plate = inVec[j];
      Mat inMat = plate.getPlateMat();
      int result = plateSetScore(plate);
      if (0 == result) {
        if (0) {
          imshow("inMat", inMat);
          waitKey(0);
          destroyWindow("inMat");
        }

        if (plate.getPlateLocateType() == CMSER) {
          int w = inMat.cols;
          int h = inMat.rows;
          Mat tmpmat = inMat(Rect_<double>(w * 0.05, h * 0.1, w * 0.9, h * 0.8));
          Mat tmpDes = inMat.clone();
          resize(tmpmat, tmpDes, Size(inMat.size()));
          plate.setPlateMat(tmpDes);
          if (useCascadeJudge) {
            int resultCascade = plateSetScore(plate);
            if (plate.getPlateLocateType() != CMSER)
              plate.setPlateMat(inMat);
            if (resultCascade == 0) {
              if (0) {
                imshow("tmpDes", tmpDes);
                waitKey(0);
                destroyWindow("tmpDes");
              }
              plateVec.push_back(plate);
            }
          }
          else 
            plateVec.push_back(plate);
        }
        else 
          plateVec.push_back(plate);                  
      }
    }

    std::vector<CPlate> reDupPlateVec;
    double overlap = 0.5;
    // double overlap = CParams::instance()->getParam1f();
    // use NMS to get the result plates
    NMS(plateVec, reDupPlateVec, overlap);
    // sort the plates due to their scores
    std::sort(reDupPlateVec.begin(), reDupPlateVec.end());
    // output the plate judge plates
    std::vector<CPlate>::iterator it = reDupPlateVec.begin();
    int count = 0;
    for (; it != reDupPlateVec.end(); ++it) {
      resultVec.push_back(*it);
      if (0) {
        imshow("plateMat", it->getPlateMat());
        waitKey(0);
        destroyWindow("plateMat");
      }
      count++;
      if (count >= maxPlates)
        break;
    }
    return 0;
  }
}
