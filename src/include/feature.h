#ifndef __FEATURE_H__
#define __FEATURE_H__

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

	//! ��ó��Ƶ�������
	Mat getTheFeatures(Mat in);

	//! EasyPR��getFeatures�ص�����
	//! ���ڴӳ��Ƶ�image����svm��ѵ������features
	typedef void(*svmCallback)(const Mat& image, Mat& features);

	//! EasyPR��getFeatures�ص�����
	//! ������������ֱ��ͼ���������Ļص�����
	void getHisteqFeatures(const Mat& image, Mat& features);

	//!  EasyPR��getFeatures�ص�����
	//! �������ǻ�ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
	void getHistogramFeatures(const Mat& image, Mat& features);

	//! �������ǻ�ȡSIFT������
	void getSIFTFeatures(const Mat& image, Mat& features);

	//! �������ǻ�ȡHOG������
	void getHOGFeatures(const Mat& image, Mat& features);

	//! �������ǻ�ȡHSV�ռ�������ֱ��ͼ������
	void getHSVHistFeatures(const Mat& image, Mat& features);

}	/*! \namespace easypr*/

#endif
/* endif __FEATURE_H__ */
