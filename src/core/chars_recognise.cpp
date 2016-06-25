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
      if (j == 0)
        isChinses = true;
      auto character = CharsIdentify::instance()->identify(charMat, isChinses);
      plateLicense.append(character.second);
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

  int result = m_charsSegment->charsSegment(plateMat, matChars);
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
      if (j == 0)
        isChinses = true;
      auto character = CharsIdentify::instance()->identify(charMat, isChinses);
      plateLicense.append(character.second);

      CCharacter charResult;
      charResult.setCharacterMat(charMat);
      charResult.setCharacterStr(character.second);

      plate.addReutCharacter(charResult);
    }

  }
  if (plateLicense.size() < 7) {
    return -1;
  }

  return result;
}


}