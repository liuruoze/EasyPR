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

#include "easypr/core/plate.h"
#include "easypr/core/core_func.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateLocate {
 public:
  CPlateLocate();

  //! Sobel第一次搜索
  //! 不限制大小和形状，获取的BoundRect进入下一步

  int sobelFrtSearch(const Mat& src, std::vector<Rect_<float>>& outRects);

  //! Sobel第二次搜索
  //! 对大小和形状做限制，生成参考坐标

  int sobelSecSearch(Mat& bound, Point2f refpoint,
                     std::vector<RotatedRect>& outRects);
  int sobelSecSearchPart(Mat& bound, Point2f refpoint,
                         std::vector<RotatedRect>& outRects);

  //! 抗扭斜处理

  int deskew(const Mat& src, const Mat& src_b,
             std::vector<RotatedRect>& inRects, std::vector<CPlate>& outPlates);

  //! 是否偏斜
  //! 输入二值化图像，输出判断结果

  bool isdeflection(const Mat& in, const double angle, double& slope);

  //! Sobel运算
  //! 输入彩色图像，输出二值化图像

  int sobelOper(const Mat& in, Mat& out, int blurSize, int morphW, int morphH);

  //! 计算一个安全的Rect

  bool calcSafeRect(const RotatedRect& roi_rect, const Mat& src,
                    Rect_<float>& safeBoundRect);

  //! 旋转操作

  bool rotation(Mat& in, Mat& out, const Size rect_size, const Point2f center,
                const double angle);

  //! 扭变操作

  void affine(const Mat& in, Mat& out, const double slope);

  //! 颜色定位法

  int plateColorLocate(Mat src, std::vector<CPlate>& candPlates, int index = 0);

  //! Sobel定位法

  int plateSobelLocate(Mat src, std::vector<CPlate>& candPlates, int index = 0);
  int sobelOperT(const Mat& in, Mat& out, int blurSize, int morphW, int morphH);

  //! Color搜索

  int colorSearch(const Mat& src, const Color r, Mat& out,
                  std::vector<RotatedRect>& outRects, int index = 0);

  //! 未使用函数与代码
  //! 开始------------
  //! 结束------------
  //! 未使用函数与代码

  //! 车牌定位

  int plateLocate(Mat, std::vector<Mat>&, int = 0);

  //! 车牌的尺寸验证

  bool verifySizes(RotatedRect mr);

  //! 生活模式与工业模式切换

  void setLifemode(bool param);

  //! 设置与读取变量

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

  //! 是否开启调试模式

  inline void setDebug(bool param) { m_debug = param; }

  //! 获取调试模式状态

  inline bool getDebug() { return m_debug; }

  //! PlateLocate所用常量

  static const int DEFAULT_GAUSSIANBLUR_SIZE = 5;
  static const int SOBEL_SCALE = 1;
  static const int SOBEL_DELTA = 0;
  static const int SOBEL_DDEPTH = CV_16S;
  static const int SOBEL_X_WEIGHT = 1;
  static const int SOBEL_Y_WEIGHT = 0;
  static const int DEFAULT_MORPH_SIZE_WIDTH = 17;  // 17
  static const int DEFAULT_MORPH_SIZE_HEIGHT = 3;  // 3

  //! showResultMat所用常量

  static const int WIDTH = 136;
  static const int HEIGHT = 36;
  static const int TYPE = CV_8UC3;

  //! verifySize所用常量

  static const int DEFAULT_VERIFY_MIN = 1;   // 3
  static const int DEFAULT_VERIFY_MAX = 24;  // 20

  //! 角度判断所用常量

  static const int DEFAULT_ANGLE = 60;  // 30

  //! 是否开启调试模式常量，默认0代表关闭

  static const int DEFAULT_DEBUG = 1;

 protected:
  //! 高斯模糊所用变量

  int m_GaussianBlurSize;

  //! 连接操作所用变量

  int m_MorphSizeWidth;
  int m_MorphSizeHeight;

  //! verifySize所用变量

  float m_error;
  float m_aspect;
  int m_verifyMin;
  int m_verifyMax;

  //! 角度判断所用变量

  int m_angle;

  //! 是否开启调试模式，0关闭，非0开启

  bool m_debug;
};

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_PLATELOCATE_H_