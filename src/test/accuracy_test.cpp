// ͨ����ȷ�ʲ����ļ�
// AcurayTest��Ӧ��main���������е�ѡ��2

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/feature.h"

using namespace easypr;

int acurayTest(const string& test_path)
{
	////��ȡ��·���µ������ļ�
	vector<string> files;
	getFiles(test_path, files);

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
		cout << "No File Found in general_test/native_test!" << endl;
		return 0;
	}

	cout << "Begin to test the easypr accuracy!" << endl;

	// �ܵĲ���ͼƬ����
	int count_all = 0;
	// �����ͼƬ����
	int count_err = 0;
	// δʶ���ͼƬ����
	int count_norecogin = 0;

	// �ܵ��ַ����
	float diff_all = 0;
	// ƽ���ַ����
	float diff_avg = 0;
	// ��ȫƥ���ʶ�����
	float match_count = 0;
	// ��ȫƥ���ʶ�������ռʶ��ͼƬ�еı���
	float match_rate = 0;

	for (int i = 0; i < size; i++)
	{
		string filepath = files[i].c_str();
		cout << "------------------" << endl;

		// ��ȡ��ʵ�ĳ���
		string plateLicense = "";
		getFileName(filepath, plateLicense);

		cout << "ԭ��:" << plateLicense << endl;

		// EasyPR��ʼ�жϳ���
		Mat src = imread(filepath);
		vector<string> plateVec;
		int result = pr.plateRecognize(src, plateVec);
		if (result == 0)
		{
			int num = plateVec.size();

			if (num == 0)
			{
				cout << ""<< "�޳���" <<endl;
				if (plateLicense != "�޳���")
					count_norecogin++;
			} 
			else if ( num > 1)
			{
				// �೵��ʹ��diff��С���Ǹ���¼
				int mindiff = 10000;
				for (int j = 0; j < num; j++)
				{
					cout << plateVec[j] << " (" << j+1 << ")"<<endl;
					string colorplate = plateVec[j];

					// ����"����:��E7KU22"��ð�ź���ĳ��ƴ�С"
					vector<string> spilt_plate;
					SplitString(colorplate, spilt_plate, ":");

					int size = spilt_plate.size();
					if (size == 2)
					{
						int diff = levenshtein_distance(plateLicense, spilt_plate[size-1]);
						if (diff < mindiff)
							mindiff = diff;
					}
				}

				cout << "���:" << mindiff << "���ַ�" << endl;
				if(mindiff == 0)
				{
					// ��ȫƥ��
					match_count++;
				}
				diff_all = diff_all + mindiff;
			}
			else
			{
				// ������ֻ����һ��diff
				for (int j = 0; j < num; j++)
				{
					cout << plateVec[j] <<endl;
					string colorplate = plateVec[j];

					// ����"����:��E7KU22"��ð�ź���ĳ��ƴ�С"
					vector<string> spilt_plate;
					SplitString(colorplate, spilt_plate, ":");

					int size = spilt_plate.size();
					if (size == 2)
					{
						int diff = levenshtein_distance(plateLicense, spilt_plate[size-1]);
						cout << "���:" << diff << "���ַ�" << endl;

						if(diff == 0)
						{
							// ��ȫƥ��
							match_count++;
						}
						diff_all = diff_all + diff;
					}
				}

			}
		} 
		else
		{
			cout << "������:" << result << endl;
			count_err++;
		}
		count_all++;
	}

	cout << "------------------" << endl;
	cout << "Easypr accuracy test end!" << endl;
	cout << "------------------" << endl;
	cout << endl;
	cout << "ͳ�Ʋ���:"  << endl;
	cout << "��ͼƬ��:" << count_all << "��,  ";
	cout << "δʶ��ͼƬ:" << count_norecogin << "��,  ";

	float count_recogin = count_all - (count_err + count_norecogin);
	float count_rate  = count_recogin / count_all;
	float count_norate = 1 - count_rate;
	cout << "ʶ����:" << count_rate * 100 << "%  " << endl;

	diff_avg = diff_all / count_recogin;
	match_rate = match_count/ count_recogin * 100;

	cout << "ƽ���ַ����:" << diff_avg << "��,  ";
	cout << "��ȫƥ����:" << match_count << "��,  ";
	cout << "��ȫƥ����:" << match_rate << "%  " << endl;
	cout << endl;

	cout << "------------------" << endl;

	return 0;
}