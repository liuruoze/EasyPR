// SVMTrain.cpp : 定义控制台应用程序的入口点。

#include <iostream>
#include <cstdlib>
#include <io.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <algorithm>

using namespace std;
using namespace cv;

void getFiles(string path, vector<string>& files );

void getPlate()
{

	char * filePath = "train/data/plate_detect_svm/HasPlate/HasPlate";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in train HasPlate!" << endl;

	////随机选取80%作为训练数据，20%作为测试数据
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	int boundry = 0.8 * size;

	cout << "Save HasPlate train!" << endl;
	for (int i = 0; i < boundry; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/HasPlate/train/hasplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}

	cout << "Save HasPlate test!" << endl;
	for (int i = boundry; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/HasPlate/test/hasplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}
}

void getNoPlate()
{

	char * filePath = "train/data/plate_detect_svm/NoPlate/NoPlate";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );
	int size = files.size();
	if (0 == size)
		cout << "No File Found in train NoPlate!" << endl;

	////随机选取80%作为训练数据，20%作为测试数据
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	int boundry = 0.8 * size;

	cout << "Save NoPlate train!" << endl;
	for (int i = 0; i < boundry; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/NoPlate/train/noplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}

	cout << "Get NoPlate test!" << endl;
	for (int i = boundry; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

		if(1)
		{ 
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/NoPlate/test/noplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}
}


void getHasPlateTrain(Mat& trainingImages, vector<int>& trainingLabels)
{
	int label = 1;
	char * filePath = "train/data/plate_detect_svm/HasPlate/train";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in train HasPlate!" << endl;

	cout << "get HasPlate train!" << endl;
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		img = img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(label);
	}
}


void getNoPlateTrain(Mat& trainingImages, vector<int>& trainingLabels)
{
	int label = 0;
	char * filePath = "train/data/plate_detect_svm/NoPlate/train";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in train HasPlate!" << endl;

	cout << "get NoPlate train!" << endl;
	for (int i = 0; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		img = img.reshape(1, 1);
        trainingImages.push_back(img);
        trainingLabels.push_back(label);
	}
}

void getHasPlateTest(vector<Mat>& testingImages, vector<int>& testingLabels)
{
	int label = 1;
	char * filePath = "train/data/plate_detect_svm/HasPlate/test";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in test HasPlate!" << endl;

	cout << "get HasPlate test!" << endl;
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		img = img.reshape(1, 1);
        testingImages.push_back(img);
        testingLabels.push_back(label);
	}
}

void getNoPlateTest(vector<Mat>& testingImages, vector<int>& testingLabels)
{
	int label = 0;
	char * filePath = "train/data/plate_detect_svm/NoPlate/test";
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files );

	int size = files.size();
	if (0 == size)
		cout << "No File Found in test NoPlate!" << endl;

	cout << "get NoPlate test!" << endl;
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		img = img.reshape(1, 1);
        testingImages.push_back(img);
        testingLabels.push_back(label);
	}
}

void getAccuracy(Mat& testingclasses_preditc, Mat& testingclasses_real)
{
	int channels = testingclasses_preditc.channels();
	cout << "channels: " << channels << endl;
	int nRows = testingclasses_preditc.rows;
	cout << "nRows: " << nRows << endl;
	int nCols = testingclasses_preditc.cols * channels;
	cout << "nCols: " << nCols << endl;

	int channels_real = testingclasses_real.channels();
	cout << "channels_real: " << channels_real << endl;
	int nRows_real = testingclasses_real.rows;
	cout << "nRows_real: " << nRows_real << endl;
	int nCols_real = testingclasses_real.cols * channels;
	cout << "nCols_real: " << nCols_real << endl;

	double count_all = 0;
	double ptrue_rtrue = 0;
	double ptrue_rfalse = 0;
	double pfalse_rtrue = 0;
	double pfalse_rfalse = 0;

	for (int i = 0; i < nRows; i++)
	{
		const uchar* inData = testingclasses_preditc.ptr<uchar>(i);
		const uchar* outData = testingclasses_real.ptr<uchar>(i);

		//float predict = testingclasses_preditc.at<float>(i);
		//float real = testingclasses_real.at<float>(i);

		float predict = inData[0];
		float real = outData[0];

		count_all ++;

		//cout << "predict:" << predict << endl;
		//cout << "real:" << real << endl;

		if (predict == 1.0 && real == 1.0)
			ptrue_rtrue ++;
		if (predict == 1.0 && real == 0)
			ptrue_rfalse ++;
		if (predict == 0 && real == 1.0)
			pfalse_rtrue ++;
		if (predict == 0 && real == 0)
			pfalse_rfalse ++;
	}

	cout << "count_all: " << count_all << endl;
	cout << "ptrue_rtrue: " << ptrue_rtrue/count_all << endl;
	cout << "ptrue_rfalse: " << ptrue_rfalse/count_all << endl;
	cout << "pfalse_rtrue: " << pfalse_rtrue/count_all << endl;
	cout << "pfalse_rfalse: " << pfalse_rfalse/count_all << endl;

	double precise = 0;
	if (ptrue_rtrue + ptrue_rfalse != 0)
	{
		precise = ptrue_rtrue/(ptrue_rtrue + ptrue_rfalse);
		cout << "precise: " << precise << endl;
	}
	else
	{
		cout << "precise: " << "NA" << endl;
	}

	double recall = 0;
	if (ptrue_rtrue + pfalse_rtrue != 0)
	{
		recall = ptrue_rtrue/(ptrue_rtrue + pfalse_rtrue);
		cout << "recall: " << recall << endl;
	}
	else
	{
		cout << "recall: " << "NA" << endl;
	}

	double F = 0;
	if (precise + recall != 0)
	{
		F = (precise * recall)/(precise + recall);
		cout << "F: " << F << endl;
	}
	else
	{
		cout << "F: " << "NA" << endl;
	}
}


int svmMain()
{

	Mat classes;//(numPlates+numNoPlates, 1, CV_32FC1);
    Mat trainingData;//(numPlates+numNoPlates, imageWidth*imageHeight, CV_32FC1 );

    Mat trainingImages;
    vector<int> trainingLabels;

	//getHasPlate();
	//getNoPlate();

	getHasPlateTrain(trainingImages, trainingLabels);
	getNoPlateTrain(trainingImages, trainingLabels);

    Mat(trainingImages).copyTo(trainingData);
    trainingData.convertTo(trainingData, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);

	CvSVMParams SVM_params;
	SVM_params.svm_type = CvSVM::C_SVC;
	SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;   线型，也就是无核
	//SVM_params.kernel_type = CvSVM::RBF; //CvSVM::RBF 径向基函数，也就是高斯核
	SVM_params.degree = 1;
	SVM_params.gamma = 1;
	SVM_params.coef0 = 1;
	SVM_params.C = 1;
	SVM_params.nu = 1;
	SVM_params.p = 1;
	SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 0.0001);

	//Train SVM
	cout << "Begin to generate svm" << endl;
	CvSVM svm(trainingData, classes, Mat(), Mat(), SVM_params);
	svm.train(trainingData, classes, Mat(), Mat(), SVM_params);

	cout << "Svm generate done!" << endl;

	//FileStorage fsTo("train/svm_0.8.xml", cv::FileStorage::WRITE);
	//svm.write(*fsTo, "svm");

	//Test SVM
	cout << "Begin to test svm" << endl;

	Mat testingclasses_real;
	Mat testingclasses_predict;
    Mat testingData;

    //Mat testingImages;
	vector<Mat> testingImages;
    vector<int> testingLabels_real;

	cout << "Get the test data" << endl;
	getHasPlateTest(testingImages, testingLabels_real);
	getNoPlateTest(testingImages, testingLabels_real);

	//Mat(testingImages).copyTo(testingData);
    //testingData.convertTo(testingData, CV_32FC1);
	//Mat(testingLabels_real).copyTo(testingclasses_real);

	cout << "Begin to predict" << endl;

	double count_all = 0;
	double ptrue_rtrue = 0;
	double ptrue_rfalse = 0;
	double pfalse_rtrue = 0;
	double pfalse_rfalse = 0;

	int size = testingImages.size();
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat p = testingImages[i];
		p = p.reshape(1, 1);
		p.convertTo(p, CV_32FC1);

		int predict = (int)svm.predict(p);
		int real = testingLabels_real[i];

		if (predict == 1 && real == 1)
			ptrue_rtrue ++;
		if (predict == 1 && real == 0)
			ptrue_rfalse ++;
		if (predict == 0 && real == 1)
			pfalse_rtrue ++;
		if (predict == 0 && real == 0)
			pfalse_rfalse ++;
	}

	count_all = double(size);

	cout << "Get the Accuracy!" << endl;

	cout << "count_all: " << count_all << endl;
	cout << "ptrue_rtrue: " << ptrue_rtrue / count_all << endl;
	cout << "ptrue_rfalse: " << ptrue_rfalse / count_all << endl;
	cout << "pfalse_rtrue: " << pfalse_rtrue / count_all << endl;
	cout << "pfalse_rfalse: " << pfalse_rfalse / count_all << endl;

	double precise = 0;
	if (ptrue_rtrue + ptrue_rfalse != 0)
	{
		precise = ptrue_rtrue / (ptrue_rtrue + ptrue_rfalse);
		cout << "precise: " << precise << endl;
	}
	else
	{
		cout << "precise: " << "NA" << endl;
	}

	double recall = 0;
	if (ptrue_rtrue + pfalse_rtrue != 0)
	{
		recall = ptrue_rtrue / (ptrue_rtrue + pfalse_rtrue);
		cout << "recall: " << recall << endl;
	}
	else
	{
		cout << "recall: " << "NA" << endl;
	}

	double Fsocre = 0;
	if (precise + recall != 0)
	{
		Fsocre = 2 * (precise * recall) / (precise + recall);
		cout << "Fsocre: " << Fsocre << endl;
	}
	else
	{
		cout << "Fsocre: " << "NA" << endl;
	}


	//svm.predict(testingData, testingclasses_predict);
	//getAccuracy(testingclasses_predict, testingclasses_real);

	int a;
	cin >> a;

	return 0;
}

