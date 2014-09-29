#include "../include/plate_locate.h"
#include "../include/plate_judge.h"
#include "../include/chars_segment.h"
#include "../include/chars_identify.h"

#include "../include/plate_detect.h"
#include "../include/chars_recognise.h"

#include "../include/plate_recognize.h"

using namespace easypr;

int test_plate_locate();
int test_plate_judge();
int test_chars_segment();
int test_chars_identify();

int test_plate_detect();
int test_chars_recognise();

int test_plate_recognize();


int main()
{
	assert (test_plate_locate() == 0);
	assert (test_plate_judge() == 0);
	assert (test_plate_detect() == 0);

	assert (test_chars_segment() == 0);
	assert (test_chars_identify() == 0);
	assert (test_chars_recognise() == 0);

	assert (test_plate_recognize() == 0);

	return 0;
}

int test_plate_locate()
{
	cout << "test_plate_locate" << endl;

	Mat src = imread("image/plate_locate.jpg");

	vector<Mat> resultVec;
	CPlateLocate plate;

	int result = plate.plateLocate(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_locate", resultMat);
			waitKey(0);
		}
	}

	return result;
}

int test_plate_judge()
{
	cout << "test_plate_judge" << endl;

	Mat src = imread("image/plate_judge.jpg");

	//可能是车牌的图块集合
	vector<Mat> matVec;

	//经过SVM判断后得到的图块集合
	vector<Mat> resultVec;

	CPlateLocate lo;
	int resultLo = lo.plateLocate(src, matVec);

	if (0 != resultLo)
		return -1;

	if (resultLo == 0)
	{
		int num = matVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = matVec[j];
			imshow("plate_judge", resultMat);
			waitKey(0);
		}
	}

	CPlateJudge ju;
	int resultJu = ju.plateJudge(matVec, resultVec);

	if (0 != resultJu)
		return -1;

	if (resultJu == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_judge", resultMat);
			waitKey(0);
		}
	}

	return resultJu;
}

int test_chars_segment()
{
	cout << "test_chars_segment" << endl;

	Mat src = imread("image/chars_segment.jpg");

	vector<Mat> resultVec;
	CCharsSegment plate;

	int result = plate.charsSegment(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("chars_segment", resultMat);
			waitKey(0);
		}
	}

	return result;
}

int test_chars_identify()
{
	cout << "test_chars_identify" << endl;

	Mat src = imread("image/chars_identify.jpg");

	vector<Mat> resultVec;
	CCharsSegment cs;
	CCharsIdentify ci;

	string plateIdentify = "";

	int result = cs.charsSegment(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			bool isChinses = false;

			//默认首个字符块是中文字符
			if (j == 0)
				isChinses = true;

			string charcater = ci.charsIdentify(resultMat, isChinses);
			plateIdentify = plateIdentify + charcater;
		}
	}

	const string plateLicense = "苏E771H6";

	cout << "plateLicense: " << plateLicense << endl;
	cout << "plateIdentify: " << plateIdentify << endl;

	if (plateLicense != plateIdentify)
	{
		cout << "Identify Not Correct!" << endl;
		return -1;
	}
	cout << "Identify Correct!" << endl;

	cout << "Enter 1 for coninue:";
	int a = 0;
	cin >> a;

	return result;
}


int test_plate_detect()
{
	cout << "test_plate_detect" << endl;

	Mat src = imread("image/plate_detect.jpg");

	vector<Mat> resultVec;
	CPlateDetect pd;

	int result = pd.plateDetect(src, resultVec);
	if (result == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_detect", resultMat);
			waitKey(0);
		}
	}

	return result;
}


int test_chars_recognise()
{
	cout << "test_chars_recognise" << endl;

	Mat src = imread("image/chars_recognise.jpg");

	CCharsRecognise cr;
	string charsRecognise = "";

	int result = cr.charsRecognise(src, charsRecognise);
	if (result == 0)
	{
		cout << "charsRecognise: " << charsRecognise << endl;
	}

	cout << "Enter 1 for coninue:";
	int a = 0;
	cin >> a;

	return result;
}

int test_plate_recognize()
{
	cout << "test_plate_recognize" << endl;

	Mat src = imread("image/plate_recognize.jpg");

	CPlateRecognize pr;
	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");

	pr.setGaussianBlurSize(5);
	pr.setMorphSizeWidth(17);

	pr.setVerifyMin(3);
	pr.setVerifyMax(20);

	pr.setLiuDingSize(7);
	pr.setColorThreshold(150);

	vector<string> plateVec;

	int result = pr.plateRecognize(src, plateVec);
	if (result == 0)
	{
		int num = plateVec.size();
		for (int j = 0; j < num; j++)
		{
			cout << "plateRecognize: " << plateVec[j] << endl;			
		}
	}

	cout << "Enter 1 for coninue:";
	int a = 0;
	cin >> a;

	return result;
}
