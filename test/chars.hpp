#ifndef EASYPR_CHARS_HPP
#define EASYPR_CHARS_HPP

namespace easypr {

namespace demo {

int test_chars_segment() {
  cout << "test_chars_segment" << endl;

  Mat src = imread("resources/image/chars_segment.jpg");

  vector <Mat> resultVec;
  CCharsSegment plate;

  int result = plate.charsSegment(src, resultVec);
  if (result == 0) {
    size_t num = resultVec.size();
    for (int j = 0; j < num; j++) {
      Mat resultMat = resultVec[j];
      imshow("chars_segment", resultMat);
      waitKey(0);
    }
    destroyWindow("chars_segment");
  }

  return result;
}

int test_chars_identify() {
  cout << "test_chars_identify" << endl;

  Mat src = imread("resources/image/chars_identify.jpg");

  vector <Mat> resultVec;
  CCharsSegment cs;
  CCharsIdentify ci;

  string plateIdentify = "";

  int result = cs.charsSegment(src, resultVec);
  if (result == 0) {
    size_t num = resultVec.size();
    for (int j = 0; j < num; j++) {
      Mat resultMat = resultVec[j];
      bool isChinses = false;
      bool isSpec = false;

      //默认首个字符块是中文字符
      if (j == 0)
        isChinses = true;

      if (j == 1)
        isSpec = true;

      string charcater = ci.charsIdentify(resultMat, isChinses, isSpec);
      plateIdentify = plateIdentify + charcater;
    }
  }

  const string plateLicense = "苏E771H6";

  cout << "plateLicense: " << plateLicense << endl;
  cout << "plateIdentify: " << plateIdentify << endl;

  if (plateLicense != plateIdentify) {
    cout << "Identify Not Correct!" << endl;
    return -1;
  }
  cout << "Identify Correct!" << endl;

  return result;
}

int test_chars_recognise() {
  cout << "test_chars_recognise" << endl;

  Mat src = imread("resources/image/chars_recognise.jpg");

  CCharsRecognise cr;
  string charsRecognise = "";

  int result = cr.charsRecognise(src, charsRecognise);
  if (result == 0) {
    cout << "charsRecognise: " << charsRecognise << endl;
  }

  return result;
}

}

}

#endif //EASYPR_CHARS_HPP
