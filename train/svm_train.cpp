// SVMTrain.cpp : 定义控制台应用程序的入口点。

#include <iostream>
#include <cstdlib>
#include <io.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>

using namespace std;
using namespace cv;

void getFiles( string path, vector<string>& files )
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

void getPlate(Mat& trainingImages, vector<int>& trainingLabels)
{

	char * filePath = "train/data/plate_detect_svm/HasPlate/HasPlate";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in train HasPlate!" << endl;

	for (int i = 0;i < size;i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
        //img= img.reshape(1, 1);
		img= img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(1);
	}
}

void getNoPlate(Mat& trainingImages, vector<int>& trainingLabels)
{

	char * filePath = "train/data/plate_detect_svm/NoPlate/NoPlate";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );
	int size = files.size();
	if (0 == size)
		cout << "No File Found in train NoPlate!" << endl;

	for (int i = 0;i < size;i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
        //img= img.reshape(1, 1);
		img= img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(0);
	}
}


int main()
{

	Mat classes;//(numPlates+numNoPlates, 1, CV_32FC1);
    Mat trainingData;//(numPlates+numNoPlates, imageWidth*imageHeight, CV_32FC1 );

    Mat trainingImages;
    vector<int> trainingLabels;

	getPlate(trainingImages, trainingLabels);
	getNoPlate(trainingImages, trainingLabels);

    Mat(trainingImages).copyTo(trainingData);
    trainingData.convertTo(trainingData, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);

	CvSVMParams SVM_params;
	SVM_params.svm_type = CvSVM::C_SVC;
	SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
	SVM_params.degree = 0;
	SVM_params.gamma = 1;
	SVM_params.coef0 = 0;
	SVM_params.C = 1;
	SVM_params.nu = 0;
	SVM_params.p = 0;
	SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);
	//Train SVM

	cout << "Begin to generate svm" << endl;
	CvSVM svmClassifier(trainingData, classes, Mat(), Mat(), SVM_params);

	cout << "Svm generate done!" << endl;

	FileStorage fsTo("train/svm.xml", cv::FileStorage::WRITE);
	svmClassifier.write(*fsTo, "svm");

	int a;
	cin >> a;

	return 0;
}

