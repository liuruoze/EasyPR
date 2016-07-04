#include "easypr/core/chars_recognise.h"
#include "easypr/core/character.hpp"
#include "easypr/util/util.h"

namespace easypr {

CCharsRecognise::CCharsRecognise() { m_charsSegment = new CCharsSegment(); }

CCharsRecognise::~CCharsRecognise() { SAFE_RELEASE(m_charsSegment); }

int CCharsRecognise::charsRecognise(Mat plate, std::string& plateLicense) {
  std::vector<Mat> matChars;

  int result = m_charsSegment->charsSegment(plate, matChars);

  //std::cout << "charsSegment:" << result << std::endl;

  if (result == 0) {
    //for (auto block : matChars) {
    //  auto character = CharsIdentify::instance()->identify(block);
    //  plateLicense.append(character.second);
    //}
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

  Mat plateMat = plate.getPlateMat();

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
  
  int result = m_charsSegment->charsSegment(plateMat, matChars, color);
  //std::cout << "charsSegment:" << result << std::endl;
  if (result == 0) {
    //for (auto block : matChars) {
    //  auto character = CharsIdentify::instance()->identify(block);
    //  plateLicense.append(character.second);
    //}
    int num = matChars.size();
    for (int j = 0; j < num; j++)
    {
      Mat charMat = matChars.at(j);
      bool isChinses = false;   
      //if (j == 0)
      //  isChinses = true;
      //auto character = CharsIdentify::instance()->identify(charMat, isChinses);
      //plateLicense.append(character.second);
      std::pair<std::string, std::string> character;
      float maxVal;
      if (j == 0) {
        isChinses = true;
        bool judge = true;
        character = CharsIdentify::instance()->identifyChinese(charMat, maxVal, judge);
        plateLicense.append(character.second);
      }
      else {
        isChinses = false;
        character = CharsIdentify::instance()->identify(charMat, isChinses);
        plateLicense.append(character.second);
      }

      CCharacter charResult;
      charResult.setCharacterMat(charMat);
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