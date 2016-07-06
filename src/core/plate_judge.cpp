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
    svm_ = ml::SVM::load<ml::SVM>(kDefaultSvmPath); 
    //svm_ = ml::SVM::load<ml::SVM>(kLBPSvmPath);   
    extractFeature = getLBPFeatures;
  }

  void PlateJudge::LoadModel(std::string path) {
    if (path != std::string(kDefaultSvmPath)) {

      if (!svm_->empty())
        svm_->clear();

      svm_ = ml::SVM::load<ml::SVM>(path);
    }
  }


  int PlateJudge::plateJudge(const Mat &inMat, int &result) {
    Mat features;
    extractFeature(inMat, features);

    float response = svm_->predict(features);
    /*std::cout << "response:" << response << std::endl;

    float score = svm_->predict(features, noArray(), cv::ml::StatModel::Flags::RAW_OUTPUT);
    std::cout << "score:" << score << std::endl;*/

    result = (int)response;

    return 0;
  }


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

  // set the score of plate
  // 0 is plate, -1 is not.
  int PlateJudge::plateSetScore(CPlate& plate) {
    Mat features;
    extractFeature(plate.getPlateMat(), features);

    float score = svm_->predict(features, noArray(), cv::ml::StatModel::Flags::RAW_OUTPUT);

    //std::cout << "score:" << score << std::endl;

    // score is the distance of margin，below zero is plate, up is not
    // when score is below zero, the samll the value, the more possibliy to be a plate.
    plate.setPlateScore(score);

    if (score < 0)
      return 0;
    else
      return -1;
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
        //Rect rectInter = rectSrc & rectComp;
        //Rect rectUnion = rectSrc | rectComp;
        //double r = double(rectInter.area()) / double(rectUnion.area());
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

  int PlateJudge::plateJudgeUsingNMS(const std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, int maxPlates) {
    std::vector<CPlate> plateVec;
    int num = inVec.size();
    bool outputResult = false;

    bool useCascadeJudge = true;
    bool useShirkMat = true;

    for (int j = 0; j < num; j++) {
      CPlate plate = inVec[j];
      Mat inMat = plate.getPlateMat();

      int result = plateSetScore(plate);

      if (result == 0) {
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
          else {
            plateVec.push_back(plate);
          }
        }
        else {
          plateVec.push_back(plate);
        }                   
      }
    }

    std::vector<CPlate> reDupPlateVec;

    double overlap = 0.7;
    //double overlap = CParams::instance()->getParam1f();
    NMS(plateVec, reDupPlateVec, overlap);
  
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
