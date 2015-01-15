#include "../include/plate_locate.h"
#include "../include/plate_judge.h"
#include "../include/chars_segment.h"
#include "../include/chars_identify.h"

#include "../include/plate_detect.h"
#include "../include/chars_recognise.h"

#include "../include/plate_recognize.h"

/**
int procMain(int argc,char **argv)
{
	cout << "test_plate_recognize" << endl;

	//Mat src = imread("image/plate_locate.jpg");
	//Mat src = imread("image/baidu_image/test1.jpg");
	Mat src = imread(argv[1]);

	CPlateRecognize pr;
	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");

	pr.setLifemode(true);

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

	return 0;
}
**/