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
int CPlateRecognize::plateRecognize(Mat src2, std::vector<CPlate> &plateVecOut, int img_index) {
  // resize to uniform sizes
  Mat img = uniformResize(src2);

  // 1. plate detect
  std::vector<CPlate> plateVec;
  int resultPD = plateDetect(img, plateVec, img_index);
  if (resultPD == 0) {
    size_t num = plateVec.size();
    int index = 0;
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

    // show results
    if (getResultShow()) {
      Mat result;
      img.copyTo(result);
      for (size_t j = 0; j < num; j++) {
        // add plates to left corner
        CPlate item = plateVec[j];
        Mat plateMat = item.getPlateMat();

        int height = 36;
        int width = 136;
        if (height * index + height < result.rows) {
          Mat imageRoi = result(Rect(0, 0 + height * index, width, height));
          addWeighted(imageRoi, 0, plateMat, 1, 0, imageRoi);
        }
        index++;

        // draw the bouding box
        RotatedRect minRect = item.getPlatePos();
        Point2f rect_points[4];
        minRect.points(rect_points);
        Scalar lineColor = Scalar(255, 255, 255);
        if (item.getPlateLocateType() == SOBEL) lineColor = Scalar(255, 0, 0);
        if (item.getPlateLocateType() == COLOR) lineColor = Scalar(0, 255, 0);
        if (item.getPlateLocateType() == CMSER) lineColor = Scalar(0, 0, 255);

        for (int j = 0; j < 4; j++)
          line(result, rect_points[j], rect_points[(j + 1) % 4], lineColor, 2, 8);
      }
      showResult(result);
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
int CPlateRecognize::plateRecognize(Mat src, std::vector<std::string> &licenseVec) {
  vector<CPlate> plates;
  int resultPR = plateRecognize(src, plates, 0);

  for (auto plate : plates) {
    licenseVec.push_back(plate.getPlateStr());
  }
  return resultPR;
}

}