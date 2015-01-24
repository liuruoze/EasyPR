// 这个文件定义了EasyPR里所有特征生成的函数
// 所属命名空间为easypr
// 这个部分中的特征由easypr的开发者修改

#include "../include/prep.h"
#include "../include/features.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

//! 直方图均衡
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

// ！获取垂直和水平方向直方图
Mat ProjectedHistogram(Mat img, int t)
{
	int sz=(t)?img.rows:img.cols;
	Mat mhist=Mat::zeros(1,sz,CV_32F);

	for(int j=0; j<sz; j++){
		Mat data=(t)?img.row(j):img.col(j);
		mhist.at<float>(j)=countNonZero(data);	//统计这一行或一列中，非零元素的个数，并保存到mhist中
	}

	//Normalize histogram
	double min, max;
	minMaxLoc(mhist, &min, &max);

	if(max>0)
		mhist.convertTo(mhist,-1 , 1.0f/max, 0);//用mhist直方图中的最大值，归一化直方图

	return mhist;
}


//! 获得车牌的特征数
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

	//Asign values to feature,样本特征为水平、垂直直方图
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

// ! EasyPR的getFeatures回调函数
// ！本函数是生成直方图均衡特征的回调函数
void getHisteqFeatures(const Mat& image, Mat& features)
{
	features = histeq(image);
}

// ! EasyPR的getFeatures回调函数
// ！本函数是获取垂直和水平的直方图图值
void getHistogramFeatures(const Mat& image, Mat& features)
{
	Mat grayImage;
	cvtColor(image, grayImage, CV_RGB2GRAY);
	//grayImage = histeq(grayImage);
	Mat img_threshold;
	threshold(grayImage, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
	features = getTheFeatures(img_threshold);
}


// ! EasyPR的getFeatures回调函数
// ！本函数是获取SITF特征子
// ! 
void getSIFTFeatures(const Mat& image, Mat& features)
{
	//待完善
}


// ! EasyPR的getFeatures回调函数
// ！本函数是获取HOG特征子
void getHOGFeatures(const Mat& image, Mat& features)
{
	//待完善
}

}	/* \namespace easypr  */