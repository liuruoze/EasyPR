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

  cv::Mat plate = cv::imread("resources/image/chars_identify.jpg");

  std::vector<Mat> matChars;
  std::string license;

  CCharsSegment cs;

  int result = cs.charsSegment(plate, matChars);
  if (result == 0) {
    for (size_t block_i = 0; block_i < matChars.size(); block_i++) {
      auto block = matChars[block_i];
      if (block_i == 0) {       
        auto character = CharsIdentify::instance()->identify(block, true);
        license.append(character.second);
      }
      else {
        auto character = CharsIdentify::instance()->identify(block, false);
        license.append(character.second);
      }
    }
    //for (auto block : matChars) {
    //  auto character = CharsIdentify::instance()->identify(block);
    //  license.append(character.second);
    //}
  }

  std::string plateLicense = "苏E771H6";
#ifdef OS_WINDOWS
  plateLicense = utils::utf8_to_gbk(plateLicense.c_str());
#endif
  std::cout << "plateLicense: " << plateLicense << std::endl;
  std::cout << "plateIdentify: " << license << std::endl;

  if (plateLicense != license) {
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
  return 0;
  std::string plateLicense = "";
  int result = cr.charsRecognise(src, plateLicense);
  if (result == 0)
    std::cout << "charsRecognise: " << plateLicense << std::endl;
  return 0;
}
}
}

#endif  // EASYPR_CHARS_HPP
