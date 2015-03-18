// ����ļ�������EasyPR�������������ɵĺ���
// ���������ռ�Ϊeasypr
// ��������е�������easypr�Ŀ������޸�

#include "../include/prep.h"
#include "../include/feature.h"
#include "../include/core_func.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

	//! ��ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
	Mat getTheFeatures(Mat in)
	{
		const int VERTICAL = 0;
		const int HORIZONTAL = 1;

		//Histogram features
		Mat vhist = ProjectedHistogram(in, VERTICAL);
		Mat hhist = ProjectedHistogram(in, HORIZONTAL);

		//Last 10 is the number of moments components
		int numCols = vhist.cols + hhist.cols;

		Mat out = Mat::zeros(1, numCols, CV_32F);

		//Asign values to feature,��������Ϊˮƽ����ֱֱ��ͼ
		int j = 0;
		for (int i = 0; i<vhist.cols; i++)
		{
			out.at<float>(j) = vhist.at<float>(i);
			j++;
		}
		for (int i = 0; i<hhist.cols; i++)
		{
			out.at<float>(j) = hhist.at<float>(i);
			j++;
		}

		return out;
	}


	//! EasyPR��getFeatures�ص�����
	//! ������������ֱ��ͼ���������Ļص�����
	void getHisteqFeatures(const Mat& image, Mat& features)
	{
		features = histeq(image);
	}

	//! EasyPR��getFeatures�ص�����
	//! �������ǻ�ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
	void getHistogramFeatures(const Mat& image, Mat& features)
	{
		Mat grayImage;
		cvtColor(image, grayImage, CV_RGB2GRAY);

		//grayImage = histeq(grayImage);

		Mat img_threshold;
		threshold(grayImage, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
		features = getTheFeatures(img_threshold);
	}


	//! EasyPR��getFeatures�ص�����
	//! �������ǻ�ȡSITF������
	void getSIFTFeatures(const Mat& image, Mat& features)
	{
		//������
	}


	//! EasyPR��getFeatures�ص�����
	//! �������ǻ�ȡHOG������
	void getHOGFeatures(const Mat& image, Mat& features)
	{
		//������
	}

	//! EasyPR��getFeatures�ص�����
	//! �������ǻ�ȡHSV�ռ�������ֱ��ͼ������
	void getHSVHistFeatures(const Mat& image, Mat& features)
	{
		//TODO
	}

}	/* \namespace easypr  */
