// mc_data_prepare.cpp : 
// ����Ԥ����ĳ�����Ҫ������������
// 1.��ȡԭ������ rawdata�����������������
// 2.���/ѡ���Ե�ѡȡ�������ݳ�Ϊlearndata��������ݲ������ã�һ������Ϊ1000��10000������1����

#include <iostream>
#include <cstdlib>
#ifdef WIN32
#include <io.h>
#else
#include <sys/io.h>
#endif
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <algorithm>

using namespace std;
using namespace cv;

const int LEARANDATA_COUNT = 5000;

void getFiles( string path, vector<string>& files );
void SplitString(const string& s, vector<string>& v, const string& c);

//! ʡ�ݶ�Ӧmap
map<string, string> mc_map;

void Code2Province(const string& code, string& province)
{
	if (mc_map.empty())
	{
		mc_map.insert(pair<string, string>("E00","δʶ��"));
		mc_map.insert(pair<string, string>("A01","��"));
		mc_map.insert(pair<string, string>("A02","��"));
		mc_map.insert(pair<string, string>("A03","��"));
		mc_map.insert(pair<string, string>("A04","��"));
		mc_map.insert(pair<string, string>("B01","��"));
		mc_map.insert(pair<string, string>("B02","��"));
		mc_map.insert(pair<string, string>("B03","��"));
		mc_map.insert(pair<string, string>("B04","��"));
		mc_map.insert(pair<string, string>("B05","��"));
		mc_map.insert(pair<string, string>("S01","��"));
		mc_map.insert(pair<string, string>("S02","��"));
		mc_map.insert(pair<string, string>("S03","��"));
		mc_map.insert(pair<string, string>("S04","��"));
		mc_map.insert(pair<string, string>("S05","��"));
		mc_map.insert(pair<string, string>("S06","��"));
		mc_map.insert(pair<string, string>("S07","��"));
		mc_map.insert(pair<string, string>("S08","��"));
		mc_map.insert(pair<string, string>("S09","��"));
		mc_map.insert(pair<string, string>("S10","ԥ"));
		mc_map.insert(pair<string, string>("S11","��"));
		mc_map.insert(pair<string, string>("S12","��"));
		mc_map.insert(pair<string, string>("S13","��"));
		mc_map.insert(pair<string, string>("S14","��"));
		mc_map.insert(pair<string, string>("S15","��"));
		mc_map.insert(pair<string, string>("S16","��"));
		mc_map.insert(pair<string, string>("S17","��"));
		mc_map.insert(pair<string, string>("S18","³"));
		mc_map.insert(pair<string, string>("S19","��"));
		mc_map.insert(pair<string, string>("S20","��"));
		mc_map.insert(pair<string, string>("S21","��"));
		mc_map.insert(pair<string, string>("S22","��"));
		mc_map.insert(pair<string, string>("J01","��"));
		mc_map.insert(pair<string, string>("J02","��"));
		mc_map.insert(pair<string, string>("J03","��"));
		mc_map.insert(pair<string, string>("J04","��"));
		mc_map.insert(pair<string, string>("J05","��"));
		mc_map.insert(pair<string, string>("J06","��"));
		mc_map.insert(pair<string, string>("J07","��"));
		mc_map.insert(pair<string, string>("J08","��"));
		mc_map.insert(pair<string, string>("J09","��"));
		mc_map.insert(pair<string, string>("J10","��"));
	}

	if (mc_map.count(code))
		province = mc_map[code];
	else
		province = "��";
}


//MC����ȥ�ϲ���ײ����ŵ�ϸ��
Mat cutBottom(Mat img)
{
	int width = img.size().width;
	int height = img.size().height;
	Rect rect(0, 0.00, width*1, height*0.97);
	Mat dst = img(rect);
	return dst;
}

//MC��filepathʾ����F:\data\easypr-data\learndata\20150110132005-210028-S18-H3952K.jpg
bool isNotNight(const string& filepath)
{
	vector<string> spilt_path;
	SplitString(filepath, spilt_path, "\\");

	int spiltsize = spilt_path.size();
	string filename = "";
	if (spiltsize != 0)
	{
		filename = spilt_path[spiltsize-1];
		if (filename != "")
		{
			vector<string> spilt_name;
			SplitString(filename, spilt_name, "-");

			int name_size = spilt_name.size();
			string datestr = "";
			if (name_size != 0)
			{
				datestr = spilt_name[0];
				if (datestr != "")
				{
					//"20150110132005", ʱ���ڵ�6��7��λ��
					//������10�㵽����2���ͼƬ��ȡ����
					string hourstr = datestr.substr(8, 2);
					if ( hourstr <= "14" && hourstr >= "10")
						return true;	
				}	
			}
		}
	}

	return false;
}

//! MC����rawdata���ļ�����·����learndata��
bool getNewPath(const string& filepath, string& newfilepath)
{
	string writePath = "F:/data/easypr-data/learndata_dl/";
	vector<string> spilt_path;
	SplitString(filepath, spilt_path, "\\");

	int spiltsize = spilt_path.size();
	string filename = "";
	if (spiltsize != 0)
	{
		filename = spilt_path[spiltsize-1];
		newfilepath = writePath + filename;
		return true;
	}
	return false;
}


//! MC��ͨ��filepath��ȡ���ƺ���
void getPlateLicense(const string& filepath, string& plateLicense)
{
	vector<string> spilt_path;
	SplitString(filepath, spilt_path, "\\");

	int spiltsize = spilt_path.size();
	string filename = "";
	if (spiltsize != 0)
	{
		filename = spilt_path[spiltsize-1];
		if (filename != "")
		{
			vector<string> spilt_name;
			SplitString(filename, spilt_name, "-");

			int name_size = spilt_name.size();
			string plateStr = "";
			string provinceCode = "";
			string provinceStr = "";
			if (name_size != 0)
			{
				// plateStr�����һ���ַ��������������".jpg"
				plateStr = spilt_name[name_size-1];

				// ��".jpg"ȥ��
				vector<string> spilt_plate;
				SplitString(plateStr, spilt_plate, ".");
				int plate_size = spilt_plate.size();
				string rawplate = "";
				if (plate_size != 0)
				{
					rawplate = spilt_plate[0];
				}

				// provinceStr�ǵ����ڶ����ַ�������Ҫ��"S13"����"��"
				provinceCode = spilt_name[name_size-2];
				Code2Province(provinceCode, provinceStr);
				plateLicense = provinceStr + rawplate;
			}
		}
	}
}


//! MC����rawdata��ȡ�������ݵ�learndata��
void getLearnData()
{
	const char * filePath = "F:/data/easypr-data/rawdata";

	////��ȡ��·���µ������ļ�
	vector<string> files;
	getFiles(filePath, files );

	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	////�������rawdata
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	////ѡȡǰLEARANDATA_COUNT��rawdata������Ϊlearndata
	int boundry = LEARANDATA_COUNT;
	int count = 0;
	cout << "Learndata saved!" << endl;
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		string filepath = files[i].c_str();

		//ֻ������������
		//if (isNotNight(filepath)!=true)
		//	continue;

		//��ȡ���ݣ�����ͼƬ����Ԥ����
		Mat img = imread(filepath);
		img = cutBottom(img);

		//�洢·���滻Ϊ�µ�
		string newfilepath = "";
		getNewPath(filepath, newfilepath);

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << newfilepath;
			imwrite(ss.str(), img);
		}

		count++;
		if (count >= boundry)
			break;
	}
}

//! ���ļ������滻���ص���MC��code�滻Ϊprovince
void changeFileName()
{
	//char * filePath = "F:/data/easypr-data/learndata";
	char * filePath = "F:/data/PlateLocate/pic1";

	////��ȡ��·���µ������ļ�
	vector<string> files;
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in learndata!" << endl;

	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		string filepath = files[i].c_str();

		//��ȡ���ݣ�����ͼƬ����Ԥ����
		Mat img = imread(filepath);
		img = cutBottom(img);

		string writePath = "F:/data/easypr-data/learn_2/";
		string plateLicense = "";
		getPlateLicense(filepath, plateLicense);

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << writePath << plateLicense << ".jpg";
			imwrite(ss.str(), img);
		}

	}
}
