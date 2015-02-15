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
    #include <sys/timeb.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "../include/util.h"

using namespace std;
using namespace easypr;

long
Utils::getTimestamp()
{
#if defined (WIN32) || defined (_WIN32)
    return GetTickCount();
#endif
    
#if (linux) || defined (__linux__)
    struct timespec ts;
    
    clock_gettime(CLOCK_MONOTONIC, &ts);
    
    return (ts.tv_sec * 1e3 + ts.tv_nsec / 1e6);
#endif
    
#if defined (__APPLE__)
    // there is no function provided by osx to get system tick count.
    // but considering the purpose by using this function,
    // we can simply return a millisecond since 1970/1/1 to calc the time elapse.
    struct timeb tb;
    ftime(&tb);
    return tb.time * 1e3 + tb.millitm;
#endif
}

std::string
Utils::getFileName(const string &path, const bool postfix /* = false */)
{
    if (!path.empty()) {
#if defined (WIN32) || defined (_WIN32)
        size_t last_slash = path.find_last_of('\\');
#else
        size_t last_slash = path.find_last_of('/');
#endif
        size_t last_dot   = path.find_last_of('.');
        
        if (last_dot < last_slash || last_dot == string::npos) {
            // not found the right dot of the postfix,
            // return the file name directly
            return path.substr(last_slash + 1);
        } else {
            // the path has a postfix
            if (postfix) {
                // return the file name including postfix
                return path.substr(last_slash + 1);
            }
            // without postfix
            return path.substr(last_slash + 1, last_dot - last_slash - 1);
        }
    }
    return "";
}

vector<string>
Utils::splitString(const string &str, const char delimiter)
{
    vector<string> splited;
    string s(str);
    size_t pos;
    
    while ((pos = s.find(delimiter)) != string::npos ) {
        string sec = s.substr(0, pos);
        
        if (!sec.empty()) {
            splited.push_back(s.substr(0, pos));
        }
        
        s = s.substr(pos + 1);
    }
    
    splited.push_back(s);
    
    return splited;
}

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
