#ifndef __FEATURES_H__
#define __FEATURES_H__

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//! ֱ��ͼ����
Mat histeq(Mat in);

// ����ȡ��ֱ��ˮƽ����ֱ��ͼ
Mat ProjectedHistogram(Mat img, int t);

//! ��ó��Ƶ�������
Mat getTheFeatures(Mat in);

// ! EasyPR��getFeatures�ص�����
// �����ڴӳ��Ƶ�image����svm��ѵ������features
typedef void(*svmCallback)(const Mat& image, Mat& features);

// ! EasyPR��getFeatures�ص�����
// ��������������ֱ��ͼ���������Ļص�����
void getHisteqFeatures(const Mat& image, Mat& features);

// ! EasyPR��getFeatures�ص�����
// ���������ǻ�ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
void getHistogramFeatures(const Mat& image, Mat& features);

// ���������ǻ�ȡSIFT������
void getSIFTFeatures(const Mat& image, Mat& features);

// ���������ǻ�ȡHOG������
void getHOGFeatures(const Mat& image, Mat& features);



}	/*! \namespace easypr*/

#endif
/* endif __FEATURES_H__ */