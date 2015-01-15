// data_prepare.cpp : 
// 数据预处理的程序，主要做以下两件事
// 1.读取原生数据 rawdata，这可能有数百万张
// 2.随机/选择性地选取部分数据成为learndata，这个根据参数设置，一般设置为1000，10000，或者1百万

#include <iostream>
#include <cstdlib>
#include <io.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace std;
using namespace cv;

const int LEARANDATA_COUNT = 1100;

void getFiles( string path, vector<string>& files );

//切去上部与底部干扰的细节
Mat cutBottom(Mat img)
{
	int width = img.size().width;
	int height = img.size().height;
	Rect rect(0, 0.00, width*1, height*0.98);
	Mat dst = img(rect);
	return dst;
}

//C++的spilt函数
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
	std::string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while(std::string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2-pos1));
 
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if(pos1 != s.length())
		v.push_back(s.substr(pos1));
}

// filepath示例：F:\data\easypr-data\learndata\20150110132005-210028-S18-H3952K.jpg
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

//! 将rawdata的文件换个路径到learndata里
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

//! 将rawdata截取部分数据到learndata中
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

void testGetLearnData()
{
	cout << "testGetLearnData!" << endl;
}
//int main()
//{
//	getLearnData();
//}