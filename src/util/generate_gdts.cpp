//	生成GDTS的文件
//	在捐赠数据到GDTS数据集里，请先用这里的方法对图像进行预处理
//	EasyPR开源项目非常注重保护原始图片的版权
//  所有的捐赠数据通过GDSL协议进行约定，保证使用人仅用于非商业性目的

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace easypr;

// TODO 将下面的路径改成你的原始图片路径
// 图片不要多，10-30张就足够了，EasyPR对GDTS数据集的使用不以量为主要指标
// 只要这些图片足够反映你数据集的主要特征即可
const string src_path = "F:/data/easypr-data/tmp-1";

// TODO 将下面的路径改成你希望生成捐赠给GDTS数据存放的新路径
const string dst_path = "F:/data/easypr-data/tmp-2";

// EasyPR的图像预处理函数，进行模糊化与裁剪化处理
Mat imageProcess(Mat src);

Mat detectAndMaskFace(Mat& img, CascadeClassifier& cascade,
                       double scale );

int generate_gdts()
{
	// 获取人脸识别文件
	CascadeClassifier cascade;
    //string cascadeName="/Users/zhoushiwei/Dropbox/Avatar/SDMLIB/haarcascade_frontalface_alt2.xml";
	string cascadeName="model/haarcascade_frontalface_alt_tree.xml";

	////获取该路径下的所有文件
	vector<string> files;
	getFiles(src_path, files);
	int size = files.size();

	if (0 == size)
	{
		cout << "No File Found!" << endl;
		return 0;
	}

	cout << "Begin to prepare generate_gdts!" << endl;

	for (int i = 0; i < size; i++)
	{
		string filepath = files[i].c_str();
		cout << "------------------" << endl;
		cout << filepath << endl;

		// EasyPR读取原图片
		Mat src = imread(filepath);

		// EasyPR开始对图片进行模糊化与裁剪化处理
		Mat img = imageProcess(src);

		// EasyPR开始对图片进行人脸识别处理
		Mat dst = detectAndMaskFace(img, cascade, 1.5);

		// 将图片导出到新路径
		vector<string> spilt_path;
		SplitString(filepath, spilt_path, "\\");
		int spiltsize = spilt_path.size();
		string filename = "";

		if (spiltsize != 0)
			filename = spilt_path[spiltsize-1];
	
		stringstream ss(stringstream::in | stringstream::out);
		ss << dst_path << "/" << filename;
		imwrite(ss.str(), dst);
	}

	return 0;
}




































// EasyPR的图像预处理函数，进行模糊化与裁剪化处理
Mat imageProcess(Mat img)
{
	int width = img.size().width;
	int height = img.size().height;
	Rect rect(width*0.01, height*0.01, width*0.99, height*0.99);

	Mat dst = img(rect);
	GaussianBlur( dst, dst, Size(1, 1), 0, 0, BORDER_DEFAULT );
	return dst;
}