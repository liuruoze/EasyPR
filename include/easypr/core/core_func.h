#ifndef EASYPR_CORE_COREFUNC_H_
#define EASYPR_CORE_COREFUNC_H_

#include "opencv2/opencv.hpp"
#include "easypr/core/plate.hpp"
#include "easypr/core/character.hpp"

using namespace cv;

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//! find binary image match to color
//! input rgb, want match color ( blue or yellow)
//! out grey, 255 is match, 0 is not match

Mat colorMatch(const Mat& src, Mat& match, const Color r,
               const bool adaptive_minsv);

//Mat mserMatch(const Mat& src, Mat& match, const Color r,
//  std::vector<RotatedRect>& plateRect, std::vector<Rect>& out_charRect);

bool plateColorJudge(const Mat& src, const Color r, const bool adaptive_minsv,
                     float& percent);

bool bFindLeftRightBound(Mat& bound_threshold, int& posLeft, int& posRight);
bool bFindLeftRightBound1(Mat& bound_threshold, int& posLeft, int& posRight);
bool bFindLeftRightBound2(Mat& bound_threshold, int& posLeft, int& posRight);

bool clearLiuDing(Mat& img);
void clearLiuDingOnly(Mat& img);
void clearLiuDing(Mat mask, int& top, int& bottom);

Color getPlateType(const Mat& src, const bool adaptive_minsv);

Mat histeq(Mat in);
Rect GetCenterRect(Mat& in);
Mat CutTheRect(Mat& in, Rect& rect);
int ThresholdOtsu(Mat mat);


Mat ProjectedHistogram(Mat img, int t);

Mat preprocessChar(Mat in, int char_size);

Rect GetChineseRect(const Rect rectSpe);

bool verifyCharSizes(Rect r);
bool verifyPlateSize(Rect mr);
bool verifyRotatedPlateSizes(RotatedRect mr, bool showDebug = false);

// non-maximum suppression
void NMStoCharacter(std::vector<CCharacter> &inVec, double overlap);

// draw rotatedRectangle
void rotatedRectangle(InputOutputArray img, RotatedRect rect,
  const Scalar& color, int thickness = 1,
  int lineType = LINE_8, int shift = 0);

// ostu region
void spatial_ostu(InputArray _src, int grid_x, int grid_y, Color type = BLUE);

// Scale to small image (for the purpose of comput mser in large image)
Mat scaleImage(const Mat& image, const Size& maxSize, double& scale_ratio);

// Scale back RotatedRect
RotatedRect scaleBackRRect(const RotatedRect& rr, const float scale_ratio);

//! use verify size to first generate char candidates
void mserCharMatch(const Mat &src, std::vector<Mat> &match, std::vector<CPlate>& out_plateVec_blue, std::vector<CPlate>& out_plateVec_yellow,
  bool usePlateMser, std::vector<RotatedRect>& out_plateRRect_blue, std::vector<RotatedRect>& out_plateRRect_yellow, int index = 0, bool showDebug = false);

// computer the insert over union about two rrect
bool computeIOU(const RotatedRect& rrect1, const RotatedRect& rrect2, const int width, const int height, const float thresh, float& result);
float computeIOU(const RotatedRect& rrect1, const RotatedRect& rrect2, const int width, const int height);

bool computeIOU(const Rect& rect1, const Rect& rect2, const float thresh, float& result);
float computeIOU(const Rect& rect1, const Rect& rect2);

/** @brief convert form mser point to image.

The function created first by Hailiang Xu.
Modified by Ruoze Liu.

@param 
*/
Mat adaptive_image_from_points(const std::vector<Point>& points,
  const Rect& rect, const Size& size, const Scalar& backgroundColor = Scalar(0, 0, 0),
  const Scalar& forgroundColor = Scalar(255, 255, 255), bool gray = true);

// Calculate a rect have same length and width and remains the center
Rect adaptive_charrect_from_rect(const Rect& rect, int maxwidth, int maxheight);

// calc safe rect
bool calcSafeRect(const RotatedRect& roi_rect, const Mat& src,
  Rect_<float>& safeBoundRect);
bool calcSafeRect(const RotatedRect &roi_rect, const int width, const int height,
  Rect_<float> &safeBoundRect);

// shift an image
Mat translateImg(Mat img, int offsetx, int offsety);

// rotate an image
Mat rotateImg(Mat source, float angle);

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_COREFUNC_H_