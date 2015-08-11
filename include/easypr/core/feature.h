#ifndef __FEATURE_H__
#define __FEATURE_H__

#include <opencv2/opencv.hpp>

namespace easypr {

  //! 获得车牌的特征数
  cv::Mat getTheFeatures(cv::Mat in);

  //! EasyPR的getFeatures回调函数
  //! 用于从车牌的image生成svm的训练特征features
  typedef void(*svmCallback)(const cv::Mat& image, cv::Mat& features);

  //! EasyPR的getFeatures回调函数
  //! 本函数是生成直方图均衡特征的回调函数
  void getHisteqFeatures(const cv::Mat& image, cv::Mat& features);

  //!  EasyPR的getFeatures回调函数
  //! 本函数是获取垂直和水平的直方图图值
  void getHistogramFeatures(const cv::Mat& image, cv::Mat& features);

  //! 本函数是获取SIFT特征子
  void getSIFTFeatures(const cv::Mat& image, cv::Mat& features);

  //! 本函数是获取HOG特征子
  void getHOGFeatures(const cv::Mat& image, cv::Mat& features);

  //! 本函数是获取HSV空间量化的直方图特征子
  void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features);

}	/*! \namespace easypr*/

#endif
/* endif __FEATURE_H__ */