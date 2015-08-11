#include "easypr/core/chars_recognise.h"
#include "easypr/util/util.h"

namespace easypr {

  CCharsRecognise::CCharsRecognise() {
    m_charsSegment = new CCharsSegment();
  }

  CCharsRecognise::~CCharsRecognise() {
    SAFE_RELEASE(m_charsSegment);
  }

  std::string CCharsRecognise::charsRecognise(Mat plate) {
    //车牌字符方块集合
    std::vector<Mat> matChars;
    std::string license;

    int result = m_charsSegment->charsSegment(plate, matChars);
    if (result == 0) {
      for(auto block : matChars){
        auto character = CharsIdentify::instance()->identify(block);
        license.append(character.second);
      }
    }
    return license;
  }

}