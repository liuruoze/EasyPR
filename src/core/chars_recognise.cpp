#include "easypr/core/chars_recognise.h"
#include "easypr/util/util.h"

namespace easypr {

CCharsRecognise::CCharsRecognise() { m_charsSegment = new CCharsSegment(); }

CCharsRecognise::~CCharsRecognise() { SAFE_RELEASE(m_charsSegment); }

int CCharsRecognise::charsRecognise(Mat plate, std::string& plateLicense) {
  std::vector<Mat> matChars;

  int result = m_charsSegment->charsSegment(plate, matChars);
  if (result == 0) {
    for (auto block : matChars) {
      auto character = CharsIdentify::instance()->identify(block);
      plateLicense.append(character.second);
    }
  }

  if (plateLicense.size() < 7) {
    return -1;
  }

  return result;

}
}