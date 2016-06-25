#ifndef EASYPR_CORE_COREFUNC_H_
#define EASYPR_CORE_COREFUNC_H_

#include <opencv2/opencv.hpp>

using namespace cv;

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

enum Color { BLUE, YELLOW, WHITE, UNKNOWN };

enum LocateType { SOBEL, COLOR, CMSER, OTHER };

enum CharSearchDirection { LEFT, RIGHT };

enum
{
  PR_DETECT_SOBEL = 0x01,  /**Sobel detect type，using twice Sobel  */
  PR_DETECT_COLOR = 0x02,  /**Color detect type   */
  PR_DETECT_CMSER = 0x04,  /**Character detect type，using mser  */
};

//! 根据一幅图像与颜色模板获取对应的二值图
//! 输入RGB图像, 颜色模板（蓝色、黄色）
//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）

Mat colorMatch(const Mat& src, Mat& match, const Color r,
               const bool adaptive_minsv);

Mat mserMatch(const Mat& src, Mat& match, const Color r,
  std::vector<RotatedRect>& plateRect, std::vector<Rect>& out_charRect);

//! 判断一个车牌的颜色
//! 输入车牌mat与颜色模板
//! 返回true或fasle

bool plateColorJudge(const Mat& src, const Color r, const bool adaptive_minsv,
                     float& percent);

bool bFindLeftRightBound(Mat& bound_threshold, int& posLeft, int& posRight);
bool bFindLeftRightBound1(Mat& bound_threshold, int& posLeft, int& posRight);
bool bFindLeftRightBound2(Mat& bound_threshold, int& posLeft, int& posRight);

//去除车牌上方的钮钉
//计算每行元素的阶跃数，如果小于X认为是柳丁，将此行全部填0（涂黑）
// X的推荐值为，可根据实际调整

bool clearLiuDing(Mat& img);
void clearLiuDingOnly(Mat& img);
void clearLiuDing(Mat mask, int& top, int& bottom);

//! 获得车牌颜色

Color getPlateType(const Mat& src, const bool adaptive_minsv);

//! 直方图均衡

Mat histeq(Mat in);
Rect GetCenterRect(Mat& in);
Mat CutTheRect(Mat& in, Rect& rect);
int ThresholdOtsu(Mat mat);

//! 获取垂直和水平方向直方图

Mat ProjectedHistogram(Mat img, int t);

Mat preprocessChar(Mat in, int char_size);

Rect GetChineseRect(const Rect rectSpe);

bool verifyCharSizes(Rect r);
bool verifyPlateSize(Rect mr);
bool verifyRotatedPlateSizes(RotatedRect mr);


void rotatedRectangle(InputOutputArray img, RotatedRect rect,
  const Scalar& color, int thickness = 1,
  int lineType = LINE_8, int shift = 0);

// Scale to small image (for the purpose of comput mser in large image)
Mat scaleImage(const Mat& image, const Size& maxSize, double& scale_ratio);

// Scale back RotatedRect
RotatedRect scaleBackRRect(const RotatedRect& rr, const float scale_ratio);

//! use verify size to first generate char candidates
Mat mserCharMatch(const Mat &src, Mat &match, std::vector<Rect>& out_charRect, Color color, int index = 0, bool showDebug = false);

/** @brief convert form mser point to image.

The function created first by Hailiang Xu.
Modified by Ruoze Liu.

@param 
*/
Mat adaptive_image_from_points(const std::vector<Point>& points,
  const Rect& rect, const Size& size, const Scalar& backgroundColor = Scalar(0, 0, 0),
  const Scalar& forgroundColor = Scalar(255, 255, 255), bool gray = true);

// Calculate a rect have same length and width and remains the center
Rect adaptive_charrect_from_rect(const Rect& rect);

//! 计算一个安全的Rect
bool calcSafeRect(const RotatedRect& roi_rect, const Mat& src,
  Rect_<float>& safeBoundRect);

// shift an image
Mat translateImg(Mat img, int offsetx, int offsety);

// rotate an image
Mat rotateImg(Mat source, float angle);

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_COREFUNC_H_