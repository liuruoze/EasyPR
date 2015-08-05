#ifndef EASYPR_CHARS_HPP
#define EASYPR_CHARS_HPP

namespace easypr {

namespace demo {

int test_chars_segment() {
  std::cout << "test_chars_segment" << std::endl;

  cv::Mat src = cv::imread("resources/image/chars_segment.jpg");

  std::vector<cv::Mat> resultVec;
  CCharsSegment plate;

  int result = plate.charsSegment(src, resultVec);
  if (result == 0) {
    size_t num = resultVec.size();
    for (size_t j = 0; j < num; j++) {
      cv::Mat resultMat = resultVec[j];
      cv::imshow("chars_segment", resultMat);
      cv::waitKey(0);
    }
    cv::destroyWindow("chars_segment");
  }

  return result;
}

int test_chars_identify() {
  std::cout << "test_chars_identify" << std::endl;

  cv::Mat src = cv::imread("resources/image/chars_identify.jpg");

  std::vector<cv::Mat> resultVec;
  CCharsSegment cs;

  std::string plateIdentify = "";

  int result = cs.charsSegment(src, resultVec);
  if (result == 0) {
    size_t num = resultVec.size();
    for (size_t j = 0; j < num; j++) {
      cv::Mat resultMat = resultVec[j];
      bool isChinses = false;
      bool isSpec = false;

      //默认首个字符块是中文字符
      if (j == 0)
        isChinses = true;

      if (j == 1)
        isSpec = true;

      std::string charcater = CharsIdentify::instance()->identify(resultMat, isChinses, isSpec);
      plateIdentify = plateIdentify + charcater;
    }
  }

  const std::string plateLicense = "苏E771H6";

  std::cout << "plateLicense: " << plateLicense << std::endl;
  std::cout << "plateIdentify: " << plateIdentify << std::endl;

  if (plateLicense != plateIdentify) {
    std::cout << "Identify Not Correct!" << std::endl;
    return -1;
  }
  std::cout << "Identify Correct!" << std::endl;

  return result;
}

int test_chars_recognise() {
  std::cout << "test_chars_recognise" << std::endl;

  cv::Mat src = cv::imread("resources/image/chars_recognise.jpg");

  CCharsRecognise cr;
  std::string charsRecognise = "";

  int result = cr.charsRecognise(src, charsRecognise);
  if (result == 0) {
    std::cout << "charsRecognise: " << charsRecognise << std::endl;
  }

  return result;
}

}

}

#endif //EASYPR_CHARS_HPP
