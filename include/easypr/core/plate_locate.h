//////////////////////////////////////////////////////////////////////////
// Name:	    plate_locate Header
// Version:		1.2
// Date:	    2014-09-19
// MDate:		2014-09-29
// MDate:	    2015-03-13
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CPlateLocate
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_PLATELOCATE_H_
#define EASYPR_CORE_PLATELOCATE_H_

#include "easypr/core/plate.hpp"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/

using namespace std;

namespace easypr {

class CPlateLocate {
 public:
  CPlateLocate();

  int sobelFrtSearch(const Mat& src, std::vector<Rect_<float>>& outRects);
  int sobelSecSearch(Mat& bound, Point2f refpoint,
                     std::vector<RotatedRect>& outRects);
  int sobelSecSearchPart(Mat& bound, Point2f refpoint,
                         std::vector<RotatedRect>& outRects);

  int deskew(const Mat& src, const Mat& src_b,
             std::vector<RotatedRect>& inRects, std::vector<CPlate>& outPlates,
             bool useDeteleArea = true, Color color = UNKNOWN);

  bool isdeflection(const Mat& in, const double angle, double& slope);

  int sobelOper(const Mat& in, Mat& out, int blurSize, int morphW, int morphH);


  bool rotation(Mat& in, Mat& out, const Size rect_size, const Point2f center,
                const double angle);

  void affine(const Mat& in, Mat& out, const double slope);

  int plateColorLocate(Mat src, std::vector<CPlate>& candPlates, int index = 0);
  int plateSobelLocate(Mat src, std::vector<CPlate>& candPlates, int index = 0);
  int sobelOperT(const Mat& in, Mat& out, int blurSize, int morphW, int morphH);

  int plateMserLocate(Mat src, std::vector<CPlate>& candPlates, int index = 0);


  int colorSearch(const Mat& src, const Color r, Mat& out,
                  std::vector<RotatedRect>& outRects);

  int mserSearch(const Mat &src, vector<Mat>& out,
    vector<vector<CPlate>>& out_plateVec, bool usePlateMser, vector<vector<RotatedRect>>& out_plateRRect,
    int img_index = 0, bool showDebug = false);

  int plateLocate(Mat, std::vector<Mat>&, int = 0);
  int plateLocate(Mat, std::vector<CPlate>&, int = 0);

  bool verifySizes(RotatedRect mr);

  void setLifemode(bool param);

  inline void setGaussianBlurSize(int param) { m_GaussianBlurSize = param; }
  inline int getGaussianBlurSize() const { return m_GaussianBlurSize; }

  inline void setMorphSizeWidth(int param) { m_MorphSizeWidth = param; }
  inline int getMorphSizeWidth() const { return m_MorphSizeWidth; }

  inline void setMorphSizeHeight(int param) { m_MorphSizeHeight = param; }
  inline int getMorphSizeHeight() const { return m_MorphSizeHeight; }

  inline void setVerifyError(float param) { m_error = param; }
  inline float getVerifyError() const { return m_error; }
  inline void setVerifyAspect(float param) { m_aspect = param; }
  inline float getVerifyAspect() const { return m_aspect; }

  inline void setVerifyMin(int param) { m_verifyMin = param; }
  inline void setVerifyMax(int param) { m_verifyMax = param; }

  inline void setJudgeAngle(int param) { m_angle = param; }

  inline void setDebug(bool param) { m_debug = param; }


  inline bool getDebug() { return m_debug; }


  static const int DEFAULT_GAUSSIANBLUR_SIZE = 5;
  static const int SOBEL_SCALE = 1;
  static const int SOBEL_DELTA = 0;
  static const int SOBEL_DDEPTH = CV_16S;
  static const int SOBEL_X_WEIGHT = 1;
  static const int SOBEL_Y_WEIGHT = 0;
  static const int DEFAULT_MORPH_SIZE_WIDTH = 17;  // 17
  static const int DEFAULT_MORPH_SIZE_HEIGHT = 3;  // 3


  static const int WIDTH = 136;
  static const int HEIGHT = 36;
  static const int TYPE = CV_8UC3;


  static const int DEFAULT_VERIFY_MIN = 1;   // 3
  static const int DEFAULT_VERIFY_MAX = 24;  // 20

  static const int DEFAULT_ANGLE = 60;  // 30


  static const int DEFAULT_DEBUG = 1;

 protected:

  int m_GaussianBlurSize;

  int m_MorphSizeWidth;
  int m_MorphSizeHeight;


  float m_error;
  float m_aspect;
  int m_verifyMin;
  int m_verifyMax;

  int m_angle;


  bool m_debug;
};

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_PLATELOCATE_H_