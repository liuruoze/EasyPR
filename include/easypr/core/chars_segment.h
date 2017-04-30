#ifndef EASYPR_CORE_CHARSSEGMENT_H_
#define EASYPR_CORE_CHARSSEGMENT_H_

#include "opencv2/opencv.hpp"
#include "easypr/config.h"

using namespace cv;
using namespace std;

namespace easypr {

class CCharsSegment {
 public:
  CCharsSegment();
  //! using ostu algotithm the segment chars in plate
  int charsSegment(Mat input, std::vector<Mat>& resultVec, Color color = BLUE);

  //! using methods to segment chars in plate
  int charsSegmentUsingOSTU(Mat input, std::vector<Mat>& resultVec, std::vector<Mat>& grayChars, Color color = BLUE);
  int charsSegmentUsingMSER(Mat input, vector<Mat>& resultVec, vector<Mat>& grayChars, Color color = BLUE);

  //! using project 
  int projectSegment(const Mat& input, Color color, vector<int>& out_indexs);

  bool verifyCharSizes(Mat r);

  // find the best chinese binaranzation method
  void judgeChinese(Mat in, Mat& out, Color plateType);
  void judgeChineseGray(Mat in, Mat& out, Color plateType);

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