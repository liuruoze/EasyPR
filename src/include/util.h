
#ifndef __UTIL_H__
#define __UTIL_H__

//C++的获取文件夹函数
void getFiles(string path, vector<string>& files);

//C++的spilt函数
void SplitString(const string& s, vector<string>& v, const string& c);

//C++的从文件路径名称到文件名称（不包括后缀）的方法
void getFileName(const string& filepath, string& name);

/// test.cpp中方法
int testMain();

/// mc_data_prepare.cpp中方法
void getLearnData();
void Code2Province(const string& code, string& province);
void changeFileName();
void getPlateLicense(const string& filepath, string& plateLicense);

/// learn_prepare.cpp中方法
void label_data();

/// accuracy_test.cpp中方法
int acurayTest();

#endif
/* endif __UTIL_H__ */