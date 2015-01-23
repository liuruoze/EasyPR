// 通用正确率测试文件
// AcurayTest对应到main控制命令中的选项2

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace easypr;

int acurayTest()
{
	////获取该路径下的所有文件
	vector<string> files;
	getFiles("image/general_test", files);

	CPlateLocate lo;
	CPlateJudge ju;
	CPlateRecognize pr;

	pr.LoadANN("model/ann.xml");
	pr.LoadSVM("model/svm.xml");
	pr.setLifemode(true);

	int size = files.size();
	//int size = 200;

	if (0 == size)
		cout << "No File Found in general_test!" << endl;

	cout << "Begin to test the easypr accuracy!" << endl;

	for (int i = 0; i < size; i++)
	{
		string filepath = files[i].c_str();
		cout << "------------------" << endl;

		// 获取真实的车牌
		string plateLicense = "";
		getFileName(filepath, plateLicense);

		cout << "原牌:" << plateLicense << endl;

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
			else if ( num > 1)
			{
				for (int j = 0; j < num; j++)
					cout << ""<< plateVec[j] << " (" << j+1 << ")"<<endl;			
			}
			else
			{
				for (int j = 0; j < num; j++)
					cout << ""<< plateVec[j] <<endl;			
			}
		} 
		else
		{
			cout << "错误码:" << result << endl;
		}

	}

	return 0;
}