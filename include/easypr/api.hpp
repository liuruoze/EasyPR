/*
 * This file provides some of the most commonly used application interfaces.
 */
#ifndef EASYPR_API_HPP
#define EASYPR_API_HPP

#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include "easypr/config.h"

namespace easypr {

namespace api {

static bool plate_judge(const char* image, const char* model) {
  cv::Mat src = cv::imread(image);

  assert(!src.empty());

  int result;
  PlateJudge::instance()->plateJudge(src, result);

  return result == 1;
}

static void plate_locate(const char* image, const bool life_mode = true) {
  cv::Mat src = cv::imread(image);

  assert(!src.empty());

  CPlateLocate plate;
  plate.setDebug(1);
  plate.setLifemode(life_mode);

  std::vector<cv::Mat> results;

  plate.plateLocate(src, results);
}

static std::vector<std::string> plate_recognize(const char* image,
                                                const char* model_svm,
                                                const char* model_ann,
                                                const bool life_mode = true) {
  cv::Mat img = cv::imread(image);

  assert(!img.empty());

  CPlateRecognize pr;
  pr.setLifemode(life_mode);
  pr.setDebug(false);

  std::vector<std::string> results;
  pr.plateRecognize(img, results);

  return std::move(results);
}

static Color get_plate_color(const char* image) {
  cv::Mat img = cv::imread(image);

  assert(!img.empty());

  return getPlateType(img, true);
}
}
}

#endif  // EASYPR_API_HPP
