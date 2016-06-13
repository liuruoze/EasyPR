#ifndef EASYPR_CORE_COREFUNC_H_
#define EASYPR_CORE_COREFUNC_H_

#include <opencv2/opencv.hpp>

using namespace cv;

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

enum Color { BLUE, YELLOW, WHITE, UNKNOWN };

enum LocateType { SOBEL, COLOR, OTHER };

//! 根据一幅图像与颜色模板获取对应的二值图
//! 输入RGB图像, 颜色模板（蓝色、黄色）
//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）

Mat colorMatch(const Mat& src, Mat& match, const Color r,
               const bool adaptive_minsv);

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
Mat features(Mat in, int sizeData);
Rect GetCenterRect(Mat& in);
Mat CutTheRect(Mat& in, Rect& rect);
int ThresholdOtsu(Mat mat);

//! 获取垂直和水平方向直方图

Mat ProjectedHistogram(Mat img, int t);

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_COREFUNC_H_