// learn data_prepare : 
// 生成learn data的cpp，learn data指的是train data，verify data， test data的总和，这个名字是我起的。
// learn data应该是贴好标签的数据，因此即便通过EasyPR自动化处理以后，也要人为的修正一下.

// 你应该按照如下的步骤和顺序组织learn data：
// 1.用EasyPR对not label data进行处理，通过EasyPR辅助来对车牌图片进行标签（放到不同文件夹下）；
// 2.标签分两种，一种是有车牌的，一种是无车牌的；
// 2.EasyPR生成标签以后，你还得需要肉眼确认下，将被不正确标签的数据转移到它们该去的文件夹下；
// 3.通过上面的步骤，正确的车牌图片和非车牌图片已经被分别放到两个文件下，假设是hasplate和noplate；
// 4.将这两个文件夹放到EasyPR目录train/data/plate_detect_svm/learn下面
// 5.运行EasyPR，选择“训练”目录下的“车牌检测（not divide）”，这个程序会自动把learn data分块，训练，测试

#include "../include/plate_recognize.h"
#include "../include/util.h"

using namespace easypr;

//! 将以下路径改成你的位置
char * notlabelPath = "F:/data/easypr-data/notlabel";
char * hasPaltePath = "F:/data/easypr-data/learn/hasPlate/";
char * noPlatePath = "F:/data/easypr-data/learn/noPlate/";

//! 读取not label data，生成learn data
void label_data()
{
	////获取该路径下的所有文件
	vector<string> files;
	getFiles(notlabelPath, files);

	CPlateLocate lo;
	CPlateJudge ju;

	int size = files.size();
	//int size = 200;

	if (0 == size)
		cout << "No File Found in notlabelPath!" << endl;

	cout << "Automatic label all the data!" << endl;

	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		string filepath = files[i].c_str();
		string writename = "";
		getPlateLicense(filepath ,writename);

		Mat src = imread(filepath);
		vector<Mat> matVec;
		int resultLo = lo.plateLocate(src, matVec);
		if (0 != resultLo)
			continue;

		int num = matVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat plate = matVec[j];
			int result;
			ju.plateJudge(plate, result);

			if(result == 1)
			{ 
				stringstream ss(stringstream::in | stringstream::out);
				ss << hasPaltePath << writename << "_" << j << ".jpg";
				imwrite(ss.str(), plate);
			}
			else
			{
				stringstream ss(stringstream::in | stringstream::out);
				ss << noPlatePath << writename << "_" << j << ".jpg";
				imwrite(ss.str(), plate);
			}
		}
	}
}

