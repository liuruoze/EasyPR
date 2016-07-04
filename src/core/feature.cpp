#include "easypr/core/feature.h"
#include "easypr/core/core_func.h"
#include "thirdparty/LBP/lbp.hpp"

namespace easypr {


Mat getHistogram(Mat in) {
  const int VERTICAL = 0;
  const int HORIZONTAL = 1;

  // Histogram features
  Mat vhist = ProjectedHistogram(in, VERTICAL);
  Mat hhist = ProjectedHistogram(in, HORIZONTAL);

  // Last 10 is the number of moments components
  int numCols = vhist.cols + hhist.cols;

  Mat out = Mat::zeros(1, numCols, CV_32F);

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

void getHistogramFeatures(const Mat& image, Mat& features) {
  Mat grayImage;
  cvtColor(image, grayImage, CV_RGB2GRAY);

  //grayImage = histeq(grayImage);

  Mat img_threshold;
  threshold(grayImage, img_threshold, 0, 255,
            CV_THRESH_OTSU + CV_THRESH_BINARY);
  features = getHistogram(img_threshold);
}


void getSIFTFeatures(const Mat& image, Mat& features) {

}


void getHOGFeatures(const Mat& image, Mat& features) {

}


void getHSVHistFeatures(const Mat& image, Mat& features) {
  // TODO
}

//! LBP feature
void getLBPFeatures(const Mat& image, Mat& features) {

  Mat grayImage;
  cvtColor(image, grayImage, CV_RGB2GRAY);

  //if (1) {
  //  imshow("grayImage", grayImage);
  //  waitKey(0);
  //  destroyWindow("grayImage");
  //}

  //spatial_ostu(grayImage, 8, 2);

  //if (1) {
  //  imshow("grayImage", grayImage);
  //  waitKey(0);
  //  destroyWindow("grayImage");
  //}

  Mat lbpimage;
  lbpimage = libfacerec::olbp(grayImage);
  Mat lbp_hist = libfacerec::spatial_histogram(lbpimage, 32, 4, 4);

  features = lbp_hist;
}

Mat charFeatures(Mat in, int sizeData) {
  const int VERTICAL = 0;
  const int HORIZONTAL = 1;

  // cut the cetner, will afect 5% perices.
  Rect _rect = GetCenterRect(in);
  Mat tmpIn = CutTheRect(in, _rect);
  //Mat tmpIn = in.clone();

  // Low data feature
  Mat lowData;
  resize(tmpIn, lowData, Size(sizeData, sizeData));

  // Histogram features
  Mat vhist = ProjectedHistogram(lowData, VERTICAL);
  Mat hhist = ProjectedHistogram(lowData, HORIZONTAL);

  // Last 10 is the number of moments components
  int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

  Mat out = Mat::zeros(1, numCols, CV_32F);
  // Asign values to

  int j = 0;
  for (int i = 0; i < vhist.cols; i++) {
    out.at<float>(j) = vhist.at<float>(i);
    j++;
  }
  for (int i = 0; i < hhist.cols; i++) {
    out.at<float>(j) = hhist.at<float>(i);
    j++;
  }
  for (int x = 0; x < lowData.cols; x++) {
    for (int y = 0; y < lowData.rows; y++) {
      out.at<float>(j) += (float)lowData.at <unsigned char>(x, y);
      j++;
    }
  }

  //std::cout << out << std::endl;

  return out;
}


Mat charFeatures2(Mat in, int sizeData) {
  const int VERTICAL = 0;
  const int HORIZONTAL = 1;

  // cut the cetner, will afect 5% perices.
  Rect _rect = GetCenterRect(in);
  Mat tmpIn = CutTheRect(in, _rect);
  //Mat tmpIn = in.clone();

  // Low data feature
  Mat lowData;
  resize(tmpIn, lowData, Size(sizeData, sizeData));

  // Histogram features
  Mat vhist = ProjectedHistogram(lowData, VERTICAL);
  Mat hhist = ProjectedHistogram(lowData, HORIZONTAL);

  // Last 10 is the number of moments components
  int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

  Mat out = Mat::zeros(1, numCols, CV_32F);
  // Asign values to

  // feature,ANN的样本特征为水平、垂直直方图和低分辨率图像所组成的矢量

  int j = 0;
  for (int i = 0; i < vhist.cols; i++) {
    out.at<float>(j) = vhist.at<float>(i);
    j++;
  }
  for (int i = 0; i < hhist.cols; i++) {
    out.at<float>(j) = hhist.at<float>(i);
    j++;
  }
  for (int x = 0; x < lowData.cols; x++) {
    for (int y = 0; y < lowData.rows; y++) {
      out.at<float>(j) += (float)lowData.at <unsigned char>(x, y);
      j++;
    }
  }

  //std::cout << out << std::endl;

  return out;
}


void getLBPplusHistFeatures(const Mat& image, Mat& features) {
  // TODO
  Mat grayImage;
  cvtColor(image, grayImage, CV_RGB2GRAY);

  Mat lbpimage;
  lbpimage = libfacerec::olbp(grayImage);
  Mat lbp_hist = libfacerec::spatial_histogram(lbpimage, 64, 8, 4);
  //features = lbp_hist.reshape(1, 1);

  Mat greyImage;
  cvtColor(image, greyImage, CV_RGB2GRAY);
  //Mat src_hsv;
  //cvtColor(image, src_hsv, CV_BGR2HSV);
  //std::vector<cv::Mat> hsvSplit;
  //split(src_hsv, hsvSplit);

  /*std::vector<cv::Mat> bgrSplit;
  split(image, bgrSplit);*/

  //grayImage = histeq(grayImage);
  Mat img_threshold;
  threshold(greyImage, img_threshold, 0, 255,
    CV_THRESH_OTSU + CV_THRESH_BINARY);
  Mat histomFeatures = getHistogram(img_threshold);

  /*Mat img_threshold2;
  threshold(bgrSplit[1], img_threshold2, 0, 255,
  CV_THRESH_OTSU + CV_THRESH_BINARY);
  Mat greenHistomFeatures = getTheFeatures(img_threshold2);

  Mat histomFeatures;
  hconcat(blueHistomFeatures.reshape(1, 1), greenHistomFeatures.reshape(1, 1), histomFeatures);*/

  //Mat histomFeatures = getTheColorFeatures(greyImage);

  //features.push_back(histomFeatures.reshape(1, 1));

  hconcat(lbp_hist.reshape(1, 1), histomFeatures.reshape(1, 1), features);
  //std::cout << features << std::endl;
  //features = histomFeatures;
}

}