#ifndef EASYPR_CORE_CHARSSEGMENT_H_
#define EASYPR_CORE_CHARSSEGMENT_H_

#include "opencv2/opencv.hpp"
#include "easypr/config.h"

using namespace cv;

namespace easypr {

class CCharsSegment {
 public:
  CCharsSegment();

  int charsSegment(Mat input, std::vector<Mat>& resultVec, Color color = BLUE);

  bool verifyCharSizes(Mat r);

  // find the best chinese binaranzation method
  void judgeChinese(Mat in, Mat& out, Color plateType);

  Mat preprocessChar(Mat in);

  //! to find the position of chinese
  Rect GetChineseRect(const Rect rectSpe);

  //! find the character refer to city, like "suA" A
  int GetSpecificRect(const std::vector<Rect>& vecRect);

  //! Do two things
  //  1.remove rect in the left of city character
  //  2.from the city rect, to the right, choose 6 rects

  int RebuildRect(const std::vector<Rect>& vecRect, std::vector<Rect>& outRect,
                  int specIndex);

  int SortRect(const std::vector<Rect>& vecRect, std::vector<Rect>& out);

  inline void setLiuDingSize(int param) { m_LiuDingSize = param; }
  inline void setColorThreshold(int param) { m_ColorThreshold = param; }

  inline void setBluePercent(float param) { m_BluePercent = param; }
  inline float getBluePercent() const { return m_BluePercent; }
  inline void setWhitePercent(float param) { m_WhitePercent = param; }
  inline float getWhitePercent() const { return m_WhitePercent; }

  static const int DEFAULT_DEBUG = 1;

  static const int CHAR_SIZE = 20;
  static const int HORIZONTAL = 1;
  static const int VERTICAL = 0;

  static const int DEFAULT_LIUDING_SIZE = 7;
  static const int DEFAULT_MAT_WIDTH = 136;
  static const int DEFAULT_COLORTHRESHOLD = 150;

  inline void setDebug(int param) { m_debug = param; }

  inline int getDebug() { return m_debug; }

 private:

  int m_LiuDingSize;

  int m_theMatWidth;

  int m_ColorThreshold;
  float m_BluePercent;
  float m_WhitePercent;

  int m_debug;
};

}

#endif  // EASYPR_CORE_CHARSSEGMENT_H_