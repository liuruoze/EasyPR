// learn data_prepare : 
// ����learn data��cpp��learn dataָ����train data��verify data�� test data���ܺͣ��������������ġ�
// learn dataӦ�������ñ�ǩ�����ݣ���˼���ͨ��EasyPR�Զ��������Ժ�ҲҪ��Ϊ������һ��.

// ��Ӧ�ð������µĲ����˳����֯learn data��
// 1.��EasyPR��not label data���д���ͨ��EasyPR�������Գ���ͼƬ���б�ǩ���ŵ���ͬ�ļ����£���
// 2.��ǩ�����֣�һ�����г��Ƶģ�һ�����޳��Ƶģ�
// 2.EasyPR���ɱ�ǩ�Ժ��㻹����Ҫ����ȷ���£���������ȷ��ǩ������ת�Ƶ����Ǹ�ȥ���ļ����£�
// 3.ͨ������Ĳ��裬��ȷ�ĳ���ͼƬ�ͷǳ���ͼƬ�Ѿ����ֱ�ŵ������ļ��£�������hasplate��noplate��
// 4.���������ļ��зŵ�EasyPRĿ¼train/data/plate_detect_svm/learn����
// 5.����EasyPR��ѡ��ѵ����Ŀ¼�µġ����Ƽ�⣨not divide���������������Զ���learn data�ֿ飬ѵ��������

#include "../include/plate_recognize.h"
#include "../include/util.h"

using namespace easypr;

//! ������·���ĳ����λ��
const char * notlabelPath = "F:/data/easypr-data/notlabel";
const char * hasPaltePath = "F:/data/easypr-data/learn/hasPlate/";
const char * noPlatePath = "F:/data/easypr-data/learn/noPlate/";

//! ��ȡnot label data������learn data
void label_data()
{
	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(notlabelPath);

	CPlateLocate lo;
	CPlateJudge ju;

	int size = files.size();

    if (0 == size) {
		cout << "File not found in " << notlabelPath << endl;
        return;
    }
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

