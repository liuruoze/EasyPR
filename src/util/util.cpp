#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace std;

void getFiles(string path, vector<string>& files)
{
	//文件句柄
	long   hFile   =   0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if((fileinfo.attrib &  _A_SUBDIR))
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles( p.assign(path).append("\\").append(fileinfo.name), files );
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
			}
		}while(_findnext(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}
}

//C++的spilt函数
void SplitString(const string& s, vector<string>& v, const string& c)
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


//! 通过文件夹名称获取文件名，不包括后缀
void getFileName(const string& filepath, string& name)
{
	vector<string> spilt_path;
	SplitString(filepath, spilt_path, "\\");

	int spiltsize = spilt_path.size();
	string filename = "";
	if (spiltsize != 0)
	{
		filename = spilt_path[spiltsize-1];

		vector<string> spilt_name;
		SplitString(filename, spilt_name, ".");

		int name_size = spilt_name.size();
		if (name_size != 0)
		{
			name = spilt_name[0];
		}
	}
}

