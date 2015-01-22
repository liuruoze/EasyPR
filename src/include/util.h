
#ifndef __UTIL_H__
#define __UTIL_H__

//C++的获取文件夹函数
void getFiles(string path, vector<string>& files);

//C++的spilt函数
void SplitString(const string& s, vector<string>& v, const string& c);

/// test.cpp中方法
int testMain();

/// mc_data_prepare.cpp中方法
void getLearnData();
void Code2Province(const string& code, string& province);
void changeFileName();

/// learn_prepare.cpp中方法
void label_data();



#endif
/* endif __UTIL_H__ */