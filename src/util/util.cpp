#include <iostream>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>
#include <io.h>
#else
#include <sys/io.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <time.h>
#include <cstring>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace std;

#ifdef WIN32
void getFiles(string path, vector<string>& files)
{
	//�ļ����
	long   hFile   =   0;
	//�ļ���Ϣ
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
	{
		do
		{
			//�����Ŀ¼,����֮
			//�������,�����б�
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
#else

void getFiles(string path, vector<string>& files) {
	DIR *dirp = opendir(path.c_str());
	if (dirp) {
		struct stat st;
		struct dirent *dir;
		char fullpath[512];
		if (dirp) {
			struct stat st;
			struct dirent *dir;
			char fullpath[512];
			while ((dir = readdir(dirp)) != NULL) {
				if (!strcmp(dir->d_name, ".") ||
					!strcmp(dir->d_name, "..")) {
						continue;
				}

				sprintf(fullpath, "%s/%s", path.c_str(), dir->d_name);

				if (lstat(fullpath, &st) < 0) {
					//perror("lstat");
					continue;
				}

				if (S_ISDIR(st.st_mode)) {
					getFiles(fullpath, files);
				} else {
					files.push_back(fullpath);
				}
			}
		}
		closedir(dirp);
	}
}
#endif

//C++��spilt����
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


//! ͨ���ļ������ƻ�ȡ�ļ�������������׺
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

