#include "easypr/core/chars_recognise.h"
#include "easypr/core/character.hpp"
#include "easypr/util/util.h"
#include <ctime>

namespace easypr {

CCharsRecognise::CCharsRecognise() { m_charsSegment = new CCharsSegment(); }

CCharsRecognise::~CCharsRecognise() { SAFE_RELEASE(m_charsSegment); }

int CCharsRecognise::charsRecognise(Mat plate, std::string& plateLicense) {
  std::vector<Mat> matChars;
  int result = m_charsSegment->charsSegment(plate, matChars);
  if (result == 0) {
    int num = matChars.size();
    for (int j = 0; j < num; j++)
    {
      Mat charMat = matChars.at(j);
      bool isChinses = false;
      float maxVal = 0;
      if (j == 0) {
        bool judge = true;
        isChinses = true;
        auto character = CharsIdentify::instance()->identifyChinese(charMat, maxVal, judge);
        plateLicense.append(character.second);
      }
      else {
        isChinses = false;
        auto character = CharsIdentify::instance()->identify(charMat, isChinses);
        plateLicense.append(character.second);
      }
    }

  }
  if (plateLicense.size() < 7) {
    return -1;
  }

  return result;
}


int CCharsRecognise::charsRecognise(CPlate& plate, std::string& plateLicense) {
  std::vector<Mat> matChars;
  std::vector<Mat> grayChars;
  Mat plateMat = plate.getPlateMat();
  if (0) writeTempImage(plateMat, "plateMat/plate");
  Color color;
  if (plate.getPlateLocateType() == CMSER) {
    color = plate.getPlateColor();
  }
  else {
    int w = plateMat.cols;
    int h = plateMat.rows;
    Mat tmpMat = plateMat(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
    color = getPlateType(tmpMat, true);
  }

  int result = m_charsSegment->charsSegmentUsingOSTU(plateMat, matChars, grayChars, color);

  if (result == 0) {
    int num = matChars.size();
    for (int j = 0; j < num; j++)
    {
      Mat charMat = matChars.at(j);
      Mat grayChar = grayChars.at(j);
      if (color != Color::BLUE)
        grayChar = 255 - grayChar;

      bool isChinses = false;
      std::pair<std::string, std::string> character;
      float maxVal;
      if (0 == j) {
        isChinses = true;
        bool judge = true;
        character = CharsIdentify::instance()->identifyChineseGray(grayChar, maxVal, judge);
        plateLicense.append(character.second);

        // set plate chinese mat and str
        plate.setChineseMat(grayChar);
        plate.setChineseKey(character.first);
        if (0) writeTempImage(grayChar, "char_data/" + character.first + "/chars_");
      }
      else if (1 == j) {
        isChinses = false;
        bool isAbc = true;
        character = CharsIdentify::instance()->identify(charMat, isChinses, isAbc);
        plateLicense.append(character.second);
      }
      else {
        isChinses = false;
        SHOW_IMAGE(charMat, 0);
        character = CharsIdentify::instance()->identify(charMat, isChinses);
        plateLicense.append(character.second);
      }

      CCharacter charResult;
      charResult.setCharacterMat(charMat);
      charResult.setCharacterGrayMat(grayChar);
      if (isChinses)
        charResult.setCharacterStr(character.first);
      else
        charResult.setCharacterStr(character.second);

      plate.addReutCharacter(charResult);
    }
    if (plateLicense.size() < 7) {
      return -1;
    }
  }

  return result;
}


}