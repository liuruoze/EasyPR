// ����ļ�������EasyPR�������������ɵĺ���
// ���������ռ�Ϊeasypr
// ��������е�������easypr�Ŀ������޸�

#include "../include/prep.h"
#include "../include/feature.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//! ֱ��ͼ����
Mat histeq(Mat in)
{
	Mat out(in.size(), in.type());
	if(in.channels()==3)
	{
		Mat hsv;
		vector<Mat> hsvSplit;
		cvtColor(in, hsv, CV_BGR2HSV);
		split(hsv, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, hsv);
		cvtColor(hsv, out, CV_HSV2BGR);
	}
	else if(in.channels()==1)
	{
		equalizeHist(in, out);
	}
	return out;
}

// ����ȡ��ֱ��ˮƽ����ֱ��ͼ
Mat ProjectedHistogram(Mat img, int t)
{
	int sz=(t)?img.rows:img.cols;
	Mat mhist=Mat::zeros(1,sz,CV_32F);

	for(int j=0; j<sz; j++){
		Mat data=(t)?img.row(j):img.col(j);
		mhist.at<float>(j)=countNonZero(data);	//ͳ����һ�л�һ���У�����Ԫ�صĸ����������浽mhist��
	}

	//Normalize histogram
	double min, max;
	minMaxLoc(mhist, &min, &max);

	if(max>0)
		mhist.convertTo(mhist,-1 , 1.0f/max, 0);//��mhistֱ��ͼ�е����ֵ����һ��ֱ��ͼ

	return mhist;
}


//! ��ó��Ƶ�������
Mat getTheFeatures(Mat in)
{
	const int VERTICAL = 0;
	const int HORIZONTAL = 1;

	//Histogram features
	Mat vhist=ProjectedHistogram(in, VERTICAL);
	Mat hhist=ProjectedHistogram(in, HORIZONTAL);

	//Last 10 is the number of moments components
	int numCols = vhist.cols + hhist.cols;

	Mat out = Mat::zeros(1, numCols, CV_32F);

	//Asign values to feature,��������Ϊˮƽ����ֱֱ��ͼ
	int j=0;
	for(int i=0; i<vhist.cols; i++)
	{
		out.at<float>(j)=vhist.at<float>(i);
		j++;
	}
	for(int i=0; i<hhist.cols; i++)
	{
		out.at<float>(j)=hhist.at<float>(i);
		j++;
	}

	return out;
}

// ! EasyPR��getFeatures�ص�����
// ��������������ֱ��ͼ���������Ļص�����
void getHisteqFeatures(const Mat& image, Mat& features)
{
	features = histeq(image);
}

// ! EasyPR��getFeatures�ص�����
// ���������ǻ�ȡ��ֱ��ˮƽ��ֱ��ͼͼֵ
void getHistogramFeatures(const Mat& image, Mat& features)
{
	Mat grayImage;
	cvtColor(image, grayImage, CV_RGB2GRAY);
	//grayImage = histeq(grayImage);
	Mat img_threshold;
	threshold(grayImage, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
	features = getTheFeatures(img_threshold);
}


// ! EasyPR��getFeatures�ص�����
// ���������ǻ�ȡSITF������
// ! 
void getSIFTFeatures(const Mat& image, Mat& features)
{
	//������
}


// ! EasyPR��getFeatures�ص�����
// ���������ǻ�ȡHOG������
void getHOGFeatures(const Mat& image, Mat& features)
{
	//������
}

}	/* \namespace easypr  */