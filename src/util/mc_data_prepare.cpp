// mc_data_prepare.cpp : 
// 数据预处理的程序，主要做以下两件事
// 1.读取原生数据 rawdata，这可能有数百万张
// 2.随机/选择性地选取部分数据成为learndata，这个根据参数设置，一般设置为1000，10000，或者1百万

#include <iostream>
#include <cstdlib>
#include <io.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <algorithm>

using namespace std;
using namespace cv;

const int LEARANDATA_COUNT = 1000;

void getFiles( string path, vector<string>& files );
void SplitString(const string& s, vector<string>& v, const string& c);

//! 省份对应map
map<string, string> mc_map;

void Code2Province(const string& code, string& province)
{
	if (mc_map.empty())
	{
		mc_map.insert(pair<string, string>("E00","未识别"));
		mc_map.insert(pair<string, string>("A01","京"));
		mc_map.insert(pair<string, string>("A02","津"));
		mc_map.insert(pair<string, string>("A03","沪"));
		mc_map.insert(pair<string, string>("A04","渝"));
		mc_map.insert(pair<string, string>("B01","桂"));
		mc_map.insert(pair<string, string>("B02","蒙"));
		mc_map.insert(pair<string, string>("B03","宁"));
		mc_map.insert(pair<string, string>("B04","新"));
		mc_map.insert(pair<string, string>("B05","藏"));
		mc_map.insert(pair<string, string>("S01","皖"));
		mc_map.insert(pair<string, string>("S02","闽"));
		mc_map.insert(pair<string, string>("S03","粤"));
		mc_map.insert(pair<string, string>("S04","甘"));
		mc_map.insert(pair<string, string>("S05","贵"));
		mc_map.insert(pair<string, string>("S06","鄂"));
		mc_map.insert(pair<string, string>("S07","冀"));
		mc_map.insert(pair<string, string>("S08","黑"));
		mc_map.insert(pair<string, string>("S09","湘"));
		mc_map.insert(pair<string, string>("S10","豫"));
		mc_map.insert(pair<string, string>("S11","琼"));
		mc_map.insert(pair<string, string>("S12","吉"));
		mc_map.insert(pair<string, string>("S13","苏"));
		mc_map.insert(pair<string, string>("S14","赣"));
		mc_map.insert(pair<string, string>("S15","辽"));
		mc_map.insert(pair<string, string>("S16","青"));
		mc_map.insert(pair<string, string>("S17","川"));
		mc_map.insert(pair<string, string>("S18","鲁"));
		mc_map.insert(pair<string, string>("S19","陕"));
		mc_map.insert(pair<string, string>("S20","晋"));
		mc_map.insert(pair<string, string>("S21","云"));
		mc_map.insert(pair<string, string>("S22","浙"));
		mc_map.insert(pair<string, string>("J01","军"));
		mc_map.insert(pair<string, string>("J02","海"));
		mc_map.insert(pair<string, string>("J03","空"));
		mc_map.insert(pair<string, string>("J04","北"));
		mc_map.insert(pair<string, string>("J05","成"));
		mc_map.insert(pair<string, string>("J06","广"));
		mc_map.insert(pair<string, string>("J07","济"));
		mc_map.insert(pair<string, string>("J08","兰"));
		mc_map.insert(pair<string, string>("J09","南"));
		mc_map.insert(pair<string, string>("J10","沈"));
	}

	if (mc_map.count(code))
		province = mc_map[code];
	else
		province = "无";
}


//MC：切去上部与底部干扰的细节
Mat cutBottom(Mat img)
{
	int width = img.size().width;
	int height = img.size().height;
	Rect rect(0, 0.00, width*1, height*0.97);
	Mat dst = img(rect);
	return dst;
}

//MC：filepath示例：F:\data\easypr-data\learndata\20150110132005-210028-S18-H3952K.jpg
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
					//"20150110132005", 时间在第6，7个位置
					//将早上10点到下午2点的图片截取出来
					string hourstr = datestr.substr(8, 2);
					if ( hourstr <= "14" && hourstr >= "10")
						return true;	
				}	
			}
		}
	}

	return false;
}

//! MC：将rawdata的文件换个路径到learndata里
bool getNewPath(const string& filepath, string& newfilepath)
{
	string writePath = "F:/data/easypr-data/learndata/";
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


//! MC：通过filepath获取车牌号码
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
				// plateStr是最后一个字符串，包括后面的".jpg"
				plateStr = spilt_name[name_size-1];

				// 将".jpg"去掉
				vector<string> spilt_plate;
				SplitString(plateStr, spilt_plate, ".");
				int plate_size = spilt_plate.size();
				string rawplate = "";
				if (plate_size != 0)
				{
					rawplate = spilt_plate[0];
				}

				// provinceStr是倒数第二个字符串，主要是"S13"代表"苏"
				provinceCode = spilt_name[name_size-2];
				Code2Province(provinceCode, provinceStr);
				plateLicense = provinceStr + rawplate;
			}
		}
	}
}


//! MC：将rawdata截取部分数据到learndata中
void getLearnData()
{
	char * filePath = "F:/data/easypr-data/rawdata";

	////获取该路径下的所有文件
	vector<string> files;
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in rawdata!" << endl;

	////随机排列rawdata
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	////选取前LEARANDATA_COUNT个rawdata数据作为learndata
	int boundry = LEARANDATA_COUNT;
	int count = 0;
	cout << "Save learndata!" << endl;
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		string filepath = files[i].c_str();

		//只处理白天的数据
		if (isNotNight(filepath)!=true)
			continue;

		//读取数据，并对图片进行预处理
		Mat img = imread(filepath);
		img = cutBottom(img);

		//存储路径替换为新的
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

//! 将文件名称替换，重点是MC的code替换为province
void changeFileName()
{
	//char * filePath = "F:/data/easypr-data/learndata";
	char * filePath = "F:/data/PlateLocate/pic1";

	////获取该路径下的所有文件
	vector<string> files;
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in learndata!" << endl;

	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		string filepath = files[i].c_str();

		//读取数据，并对图片进行预处理
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