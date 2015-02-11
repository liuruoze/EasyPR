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
int testMain();


//把你要测试的图片地址写在下面
const string test_img = "";


const string testOption[] = 
	{
		"1. test plate_locate(车牌定位);"		/* 车牌定位 */,
		"2. test plate_judge(车牌判断);"		/* 车牌判断 */,  
		"3. test plate_detect(车牌检测);"		/* 车牌检测（包含车牌定位与车牌判断） */, 
		"4. test chars_segment(字符分隔);"		/* 字符分隔 */, 
		"5. test chars_identify(字符鉴别);"		/* 字符鉴别 */,  
		"6. test chars_recognise(字符识别);"		/* 字符识别（包含字符分隔与字符鉴别） */,
		"7. test plate_recognize(车牌识别);"		/* 车牌识别 */, 
		"8. test all(测试全部);"		/* 以上全部 */,
		"9. 返回;"		/* 退出 */,
	};

const int testOptionCount = 9;

int testMain()
{
	bool isExit = false;
	while (isExit != true)
	{
		stringstream selectOption(stringstream::in | stringstream::out);
		selectOption << "EasyPR Test:"<< endl;
		for(int i = 0; i < testOptionCount; i++)
		{
			selectOption << testOption[i] << endl;
		}

		cout << "////////////////////////////////////"<< endl;
		cout << selectOption.str();
		cout << "////////////////////////////////////"<< endl;
		cout << "请选择一项操作:";

		int select = -1;
		bool isRepeat = true;
		while (isRepeat)
		{
			cin >> select;
			isRepeat = false;
			switch (select)
			{
			case 1:
				assert (test_plate_locate() == 0);
				break;
			case 2:
				assert (test_plate_judge() == 0);
				break;
			case 3:
				assert (test_plate_detect() == 0);
				break;
			case 4:
				assert (test_chars_segment() == 0);
				break;
			case 5:
				assert (test_chars_identify() == 0);
				break;
			case 6:
				assert (test_chars_recognise() == 0);
				break;
			case 7:
				assert (test_plate_recognize() == 0);
				break;
			case 8:
				assert (test_plate_locate() == 0);
				assert (test_plate_judge() == 0);
				assert (test_plate_detect() == 0);

				assert (test_chars_segment() == 0);
				assert (test_chars_identify() == 0);
				assert (test_chars_recognise() == 0);

				assert (test_plate_recognize() == 0);
				break;
			case 9:
				isExit = true;
				break;
			default:
				cout << "输入错误，请重新输入:";
				isRepeat = true;
				break;
			}
		}
	}

	return 0;
}


int test_plate_locate()
{
	cout << "test_plate_locate" << endl;

	Mat src = imread("image/plate_locate.jpg");

	vector<Mat> resultVec;
	CPlateLocate plate;
	plate.setDebug(1);
	plate.setLifemode(true);

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
        destroyWindow("plate_locate");
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
	lo.setDebug(1);
	lo.setLifemode(true);

	int resultLo = lo.plateLocate(src, matVec);

	if (0 != resultLo)
		return -1;

	cout << "plate_locate_img" << endl;
	if (resultLo == 0)
	{
		int num = matVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = matVec[j];
			imshow("plate_judge", resultMat);
			waitKey(0);
		}
        destroyWindow("plate_judge");
	}

	CPlateJudge ju;
	int resultJu = ju.plateJudge(matVec, resultVec);

	if (0 != resultJu)
		return -1;

	cout << "plate_judge_img" << endl;
	if (resultJu == 0)
	{
		int num = resultVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat resultMat = resultVec[j];
			imshow("plate_judge", resultMat);
			waitKey(0);
		}
        destroyWindow("plate_judge");
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
        destroyWindow("chars_segment");
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

	return result;
}


int test_plate_detect()
{
	cout << "test_plate_detect" << endl;

	//Mat src = imread("image/plate_detect.jpg");
	Mat src = imread("image/baidu_image/test1.jpg");

	vector<Mat> resultVec;
	CPlateDetect pd;
	pd.setPDLifemode(true);

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
        destroyWindow("plate_detect");
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

	return result;
}

int test_plate_recognize()
{
	cout << "test_plate_recognize" << endl;

	Mat src = imread("image/test.jpg");

	CPlateRecognize pr;
	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");

	pr.setLifemode(true);
	pr.setDebug(true);

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

	if (result != 0)
		cout << "result:" << result << endl;

	return result;
}
