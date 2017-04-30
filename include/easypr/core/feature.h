#ifndef EASYPR_CORE_FEATURE_H_
#define EASYPR_CORE_FEATURE_H_

#include "opencv2/opencv.hpp"

using namespace cv;

namespace easypr {

//! 获得车牌的特征数
cv::Mat getHistogram(cv::Mat in);

//! EasyPR的getFeatures回调函数
//! 用于从车牌的image生成svm的训练特征features
typedef void (*svmCallback)(const cv::Mat& image, cv::Mat& features);

//! EasyPR的getFeatures回调函数
//! convert from images to features used by gray char ann
typedef void (*annCallback)(const cv::Mat& image, cv::Mat& features);

//! gray and project feature
void getGrayPlusProject(const cv::Mat& grayChar, cv::Mat& features);

//!  EasyPR的getFeatures回调函数
//! 本函数是获取垂直和水平的直方图图值
void getHistogramFeatures(const cv::Mat& image, cv::Mat& features);

//! 本函数是获取SIFT特征子
void getSIFTFeatures(const cv::Mat& image, cv::Mat& features);

//! 本函数是获取HOG特征子
void getHOGFeatures(const cv::Mat& image, cv::Mat& features);

//! 本函数是获取HSV空间量化的直方图特征子
void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features);

//! LBP feature
void getLBPFeatures(const cv::Mat& image, cv::Mat& features);

//! color feature
void getColorFeatures(const cv::Mat& src, cv::Mat& features);

//! color feature and histom
void getHistomPlusColoFeatures(const cv::Mat& image, cv::Mat& features);

//! get character feature
cv::Mat charFeatures(cv::Mat in, int sizeData);
cv::Mat charFeatures2(cv::Mat in, int sizeData);

//! LBP feature + Histom feature
void getLBPplusHistFeatures(const cv::Mat& image, cv::Mat& features);

//! grayChar feauter
void getGrayCharFeatures(const cv::Mat& grayChar, cv::Mat& features);

void getGrayPlusLBP(const Mat& grayChar, Mat& features);
} /*! \namespace easypr*/

#endif  // EASYPR_CORE_FEATURE_H_