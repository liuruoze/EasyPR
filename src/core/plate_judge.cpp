#include "../include/plate_judge.h"
#include "../include/util.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

CPlateJudge::CPlateJudge()
{
	//cout << "CPlateJudge" << endl;
	m_path = "model/svm.xml";
	m_getFeatures = getHistogramFeatures;

	LoadModel();
}

void CPlateJudge::LoadModel()
{
	svm.clear();
	svm.load(m_path.c_str(), "svm");
}

void CPlateJudge::LoadModel(string s)
{
	svm.clear();
	svm.load(s.c_str(), "svm");
}

//! 直方图均衡
Mat CPlateJudge::histeq(Mat in)
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


//! 对单幅图像进行SVM判断
int CPlateJudge::plateJudge(const Mat& inMat,int& result)
{
	if (m_getFeatures == NULL)
		return -1;

	Mat features;
	m_getFeatures(inMat, features);

	//通过直方图均衡化后的彩色图进行预测
	Mat p = features.reshape(1, 1);
	p.convertTo(p, CV_32FC1);

	int response = (int)svm.predict(p);
	result = response;

	return 0;
}


//! 对多幅图像进行SVM判断
int CPlateJudge::plateJudge(const vector<Mat>& inVec,
								  vector<Mat>& resultVec)
{
	int num = inVec.size();
	for (int j = 0; j < num; j++)
	{
		Mat inMat = inVec[j];

		int response = -1;
		plateJudge(inMat, response);

		if (response == 1)
			resultVec.push_back(inMat);
	}
	return 0;
}


}	/*! \namespace easypr*/
