#include "easypr/core/plate_recognize.h"
#include "easypr/config.h"
#include "thirdparty/textDetect/erfilter.hpp"

namespace easypr {

CPlateRecognize::CPlateRecognize() { 
  m_showResult = false;
}


// main method, plate recognize, contain two parts
// 1. plate detect
// 2. chars recognize
int CPlateRecognize::plateRecognize(const Mat& src, std::vector<CPlate> &plateVecOut, int img_index) {
  // resize to uniform sizes
  // TODO: groungTruth affect
  Mat img = uniformResize(src);

  if (0) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << "resources/image/tmp/phone/phone_img_" << img_index << ".jpg";
    imwrite(ss.str(), img);
  }

  // 1. plate detect
  std::vector<CPlate> plateVec;
  int resultPD = plateDetect(img, plateVec, img_index);
  if (resultPD == 0) {
    size_t num = plateVec.size();
    for (size_t j = 0; j < num; j++) {
      CPlate item = plateVec.at(j);
      Mat plateMat = item.getPlateMat();
      if (0) {
        imshow("plate", plateMat);
        waitKey(0);
        destroyWindow("plate");
      }

      // get plate color
      Color color = item.getPlateColor();
      if (color == UNKNOWN) {
        color = getPlateType(plateMat, true);
        item.setPlateColor(color);
      }
      std::string plateColor = getPlateColor(color);
      if (0) {
        std::cout << "plateColor:" << plateColor << std::endl;
      }

      // 2. chars recognize
      std::string plateIdentify = "";
      int resultCR = charsRecognise(item, plateIdentify);
      if (resultCR == 0) {
        std::string license = plateColor + ":" + plateIdentify;
        item.setPlateStr(license);
        plateVecOut.push_back(item);
      }
      else {
        std::string license = plateColor;
        item.setPlateStr(license);
        plateVecOut.push_back(item);
        if (0) {
          std::cout << "resultCR:" << resultCR << std::endl;
        }
      }
    }
    if (getResultShow()) {
      // param type: 0 detect, 1 recognize;
      int showType = 0;
      if (0 == showType)
        showDectectResults(img, plateVec, num);
      else
        showDectectResults(img, plateVecOut, num);
    }
  }
  return resultPD;
}

void CPlateRecognize::LoadSVM(std::string path) {
  PlateJudge::instance()->LoadModel(path);
}

void CPlateRecognize::LoadANN(std::string path) {
  CharsIdentify::instance()->LoadModel(path);
}

void CPlateRecognize::LoadChineseANN(std::string path) {
  CharsIdentify::instance()->LoadChineseModel(path);
}

// deprected
int CPlateRecognize::plateRecognize(const Mat& src, std::vector<std::string> &licenseVec) {
  vector<CPlate> plates;
  int resultPR = plateRecognize(src, plates, 0);

  for (auto plate : plates) {
    licenseVec.push_back(plate.getPlateStr());
  }
  return resultPR;
}

}