#ifndef __FEATURES_H__
#define __FEATURES_H__

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//! 直方图均衡
Mat histeq(Mat in);

// ！获取垂直和水平方向直方图
Mat ProjectedHistogram(Mat img, int t);

//! 获得车牌的特征数
Mat getTheFeatures(Mat in);

// ! EasyPR的getFeatures回调函数
// ！用于从车牌的image生成svm的训练特征features
typedef void(*svmCallback)(const Mat& image, Mat& features);

// ! EasyPR的getFeatures回调函数
// ！本函数是生成直方图均衡特征的回调函数
void getHisteqFeatures(const Mat& image, Mat& features);

// ! EasyPR的getFeatures回调函数
// ！本函数是获取垂直和水平的直方图图值
void getHistogramFeatures(const Mat& image, Mat& features);

// ！本函数是获取SIFT特征子
void getSIFTFeatures(const Mat& image, Mat& features);

// ！本函数是获取HOG特征子
void getHOGFeatures(const Mat& image, Mat& features);



}	/*! \namespace easypr*/

#endif
/* endif __FEATURES_H__ */