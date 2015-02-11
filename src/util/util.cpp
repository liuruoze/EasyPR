#if defined (WIN32) || defined (_WIN32)
    #include <windows.h>
#endif

#include <iostream>
#include <cstdlib>

#if defined (WIN32) || defined (_WIN32)
    #include <io.h>
#elif defined (linux) || defined (__linux__)
    #include <sys/io.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <time.h>
    #include <cstring>
#elif defined (__APPLE__)
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace std;

#if defined (WIN32) || defined (_WIN32)

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

#elif defined (linux) || defined (__linux__) || defined (__APPLE__)

void getFiles(string path, vector<string>& files) {
    DIR *dirp = opendir(path.c_str());
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

#endif

#if (linux) || defined (__linux__)
double GetTickCount() {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec * 1e3 + ts.tv_nsec / 1e6);
}
#endif


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

