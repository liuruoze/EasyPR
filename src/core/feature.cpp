// 这个文件定义了EasyPR里所有特征生成的函数
// 所属命名空间为easypr
// 这个部分中的特征由easypr的开发者修改

#include "easypr/core/feature.h"
#include "easypr/core/core_func.h"

namespace easypr {

//! 获取垂直和水平的直方图图值

Mat getTheFeatures(Mat in) {
  const int VERTICAL = 0;
  const int HORIZONTAL = 1;

  // Histogram features
  Mat vhist = ProjectedHistogram(in, VERTICAL);
  Mat hhist = ProjectedHistogram(in, HORIZONTAL);

  // Last 10 is the number of moments components
  int numCols = vhist.cols + hhist.cols;

  Mat out = Mat::zeros(1, numCols, CV_32F);

  // Asign values to feature,样本特征为水平、垂直直方图

  int j = 0;
  for (int i = 0; i < vhist.cols; i++) {
    out.at<float>(j) = vhist.at<float>(i);
    j++;
  }
  for (int i = 0; i < hhist.cols; i++) {
    out.at<float>(j) = hhist.at<float>(i);
    j++;
  }

  return out;
}

//! EasyPR的getFeatures回调函数
//! 本函数是获取垂直和水平的直方图图值

void getHistogramFeatures(const Mat& image, Mat& features) {
  Mat grayImage;
  cvtColor(image, grayImage, CV_RGB2GRAY);

  //grayImage = histeq(grayImage);

  Mat img_threshold;
  threshold(grayImage, img_threshold, 0, 255,
            CV_THRESH_OTSU + CV_THRESH_BINARY);
  features = getTheFeatures(img_threshold);
}

//! EasyPR的getFeatures回调函数
//! 本函数是获取SITF特征子

void getSIFTFeatures(const Mat& image, Mat& features) {

  //待完善

}

//! EasyPR的getFeatures回调函数
//! 本函数是获取HOG特征子

void getHOGFeatures(const Mat& image, Mat& features) {

  //待完善

}

//! EasyPR的getFeatures回调函数
//! 本函数是获取HSV空间量化的直方图特征子

void getHSVHistFeatures(const Mat& image, Mat& features) {
  // TODO
}

}