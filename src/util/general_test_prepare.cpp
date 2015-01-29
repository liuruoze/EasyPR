// 辅助生成general_test中的图片
// 通用做法是使用easypr依次读取图片，并用其中的一个车牌识别文字给图片重命名
// easypr识别的文字不一定准确，因此需要人再次确认与手工调整下

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace easypr;

extern const string GENERAL_TEST_PATH;

// TODO 将下面的路径改成你的
const string your_data_path = "F:/data/easypr-data/tmp-2";

int general_test()
{
	////获取该路径下的所有文件
	vector<string> files;
	getFiles(your_data_path, files);

	CPlateLocate lo;
	CPlateJudge ju;
	CPlateRecognize pr;

	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");
	pr.setLifemode(true);

	int size = files.size();
	//int size = 200;

	if (0 == size)
	{
		cout << "No File Found!" << endl;
		return 0;
	}

	cout << "Begin to prepare general_test!" << endl;

	for (int i = 0; i < size; i++)
	{
		string filepath = files[i].c_str();
		cout << "------------------" << endl;

		// EasyPR开始判断车牌
		Mat src = imread(filepath);
		vector<string> plateVec;

		int result = pr.plateRecognize(src, plateVec);
		if (result == 0)
		{
			int num = plateVec.size();

			if (num == 0)
			{
				cout << ""<< "无车牌" <<endl;
			} 
			else 
			{
				cout << plateVec[0] <<endl;
				string colorplate = plateVec[0];

				// 输出"蓝牌:苏E7KU22"中冒号后面的车牌
				vector<string> spilt_plate;
				SplitString(colorplate, spilt_plate, ":");

				int size = spilt_plate.size();
				if (size == 2)
				{
					stringstream ss(stringstream::in | stringstream::out);
					ss << "F:/data/easypr-data/tmp-4" << "/" << spilt_plate[size-1] << ".jpg";
					imwrite(ss.str(), src);
				}
			}
		} 
		else
		{
			cout << "错误码:" << result << endl;
		}
	}

	return 0;
}