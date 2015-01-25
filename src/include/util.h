
#ifndef __UTIL_H__
#define __UTIL_H__

//C++的获取文件夹函数
void getFiles(string path, vector<string>& files);

//C++的spilt函数
void SplitString(const string& s, vector<string>& v, const string& c);

//C++的从文件路径名称到文件名称（不包括后缀）的方法
void getFileName(const string& filepath, string& name);

//! levenshtein距离，用于计算两个车牌的距离
//！EasyPR中用levenshtein距离衡量车牌识别与真实车牌的误差
template<class T>
unsigned int levenshtein_distance(const T &s1, const T & s2) {
	const size_t len1 = s1.size(), len2 = s2.size();
	vector<unsigned int> col(len2+1), prevCol(len2+1);
 
	for (unsigned int i = 0; i < prevCol.size(); i++)
		prevCol[i] = i;
	for (unsigned int i = 0; i < len1; i++) {
		col[0] = i+1;
		for (unsigned int j = 0; j < len2; j++)
			col[j+1] = min( min(prevCol[1 + j] + 1, col[j] + 1), \
								prevCol[j] + (s1[i]==s2[j] ? 0 : 1) );
		col.swap(prevCol);
	}
	return prevCol[len2];
}


/// test.cpp中方法
int testMain();

/// accuracy_test.cpp中方法
int acurayTest(const string&);

/// mc_data_prepare.cpp中方法
void getLearnData();
void Code2Province(const string& code, string& province);
void changeFileName();
void getPlateLicense(const string& filepath, string& plateLicense);

/// learn_prepare.cpp中方法
void label_data();

/// general_test_prepare.cpp中方法
int general_test();


//	deface.cpp中方法
//	反人脸识别函数
//	在发布数据到general_test数据集里，请先用这里的方法对图像进行处理
//	EasyPR开源项目非常注重保护图片中驾驶人的隐私
int deface();

/// generate_gdts.cpp中方法
int generate_gdts();

#endif
/* endif __UTIL_H__ */