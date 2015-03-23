// svm_train.cpp : svmģ�͵�ѵ���ļ�����Ҫ����plate_detect��

#include "../include/plate_recognize.h"
#include "../include/feature.h"
#include "../include/util.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

void learn2HasPlate(float bound = 0.7)
{

	const char * filePath = "train/data/plate_detect_svm/learn/HasPlate";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(filePath);

	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	////���ѡȡ70%��Ϊѵ�����ݣ�30%��Ϊ��������
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	int boundry = bound * size;

	cout << "Save HasPlate train!" << endl;
	for (int i = 0; i < boundry; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		if(1)
		{
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/train/HasPlate/hasplate_" << i << ".jpg";
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
			ss << "train/data/plate_detect_svm/test/HasPlate/hasplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}
}

void learn2NoPlate(float bound = 0.7)
{

	const char * filePath = "train/data/plate_detect_svm/learn/NoPlate";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(filePath);
	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	////���ѡȡ70%��Ϊѵ�����ݣ�30%��Ϊ��������
	srand(unsigned(time(NULL)));
	random_shuffle(files.begin(), files.end());

	int boundry = bound * size;

	cout << "Save NoPlate train!" << endl;
	for (int i = 0; i < boundry; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		if(1)
		{
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/train/NoPlate/noplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}

	cout << "Save NoPlate test!" << endl;
	for (int i = boundry; i < size; i++)
	{
		cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());
		if(1)
		{
			stringstream ss(stringstream::in | stringstream::out);
			ss << "train/data/plate_detect_svm/test/NoPlate/noplate_" << i << ".jpg";
			imwrite(ss.str(), img);
		}
	}
}

void getHasPlateTrain(Mat& trainingImages, vector<int>& trainingLabels,
	svmCallback getFeatures = getHisteqFeatures)
{
	int label = 1;
	const char * filePath = "train/data/plate_detect_svm/train/HasPlate";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(filePath);

	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	cout << "get HasPlate train!" << endl;
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

		//���ûص�������������
		Mat features;
		getFeatures(img, features);
		features = features.reshape(1, 1);

        trainingImages.push_back(features);
        trainingLabels.push_back(label);
	}
}


void getNoPlateTrain(Mat& trainingImages, vector<int>& trainingLabels,
	svmCallback getFeatures = getHisteqFeatures)
{
	int label = 0;
	const char * filePath = "train/data/plate_detect_svm/train/NoPlate";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(filePath);

	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	cout << "get NoPlate train!" << endl;
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

		//���ûص�������������
		Mat features;
		getFeatures(img, features);
		features = features.reshape(1, 1);

        trainingImages.push_back(features);
        trainingLabels.push_back(label);
	}
}

void getHasPlateTest(vector<Mat>& testingImages, vector<int>& testingLabels)
{
	int label = 1;
	const char * filePath = "train/data/plate_detect_svm/test/HasPlate";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(filePath);

	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	cout << "get HasPlate test!" << endl;
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

        testingImages.push_back(img);
        testingLabels.push_back(label);
	}
}

void getNoPlateTest(vector<Mat>& testingImages, vector<int>& testingLabels)
{
	int label = 0;
	const char * filePath = "train/data/plate_detect_svm/test/NoPlate";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(filePath);

	int size = files.size();
    if (0 == size) {
		cout << "File not found in " << filePath << endl;
        return;
    }
	cout << "get NoPlate test!" << endl;
	for (int i = 0; i < size; i++)
	{
		//cout << files[i].c_str() << endl;
		Mat img = imread(files[i].c_str());

        testingImages.push_back(img);
        testingLabels.push_back(label);
	}
}


//! ����SVM��׼ȷ�ʣ��ع����Լ�FScore
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
	cout << "ptrue_rtrue: " << ptrue_rtrue << endl;
	cout << "ptrue_rfalse: " << ptrue_rfalse << endl;
	cout << "pfalse_rtrue: " << pfalse_rtrue << endl;
	cout << "pfalse_rfalse: " << pfalse_rfalse << endl;

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


int svmTrain(bool dividePrepared = true, bool trainPrepared = true,
	svmCallback getFeatures = getHistogramFeatures)
{

	Mat classes;
    Mat trainingData;

    Mat trainingImages;
    vector<int> trainingLabels;

	if (dividePrepared == false)
	{
		//�ָ�learn������ݵ�train��test��
		cout << "Divide learn to train and test" << endl;
		learn2HasPlate();
		learn2NoPlate();
	}

	//��ѵ�����ݼ������ڴ�
	if (trainPrepared == false)
	{
		cout << "Begin to get train data to memory" << endl;
		getHasPlateTrain(trainingImages, trainingLabels, getFeatures);
		getNoPlateTrain(trainingImages, trainingLabels, getFeatures);

		Mat(trainingImages).copyTo(trainingData);
		trainingData.convertTo(trainingData, CV_32FC1);
		Mat(trainingLabels).copyTo(classes);
	}

	//Test SVM
	vector<Mat> testingImages;
    vector<int> testingLabels_real;

	//���������ݼ������ڴ�
	cout << "Begin to get test data to memory" << endl;
	getHasPlateTest(testingImages, testingLabels_real);
	getNoPlateTest(testingImages, testingLabels_real);

	CvSVM svm;
	if (trainPrepared == false && !classes.empty() && !trainingData.empty())
	{
		CvSVMParams SVM_params;
		SVM_params.svm_type = CvSVM::C_SVC;
		//SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;   ���ͣ�Ҳ�����޺�
		SVM_params.kernel_type = CvSVM::RBF; //CvSVM::RBF �����������Ҳ���Ǹ�˹��
		SVM_params.degree = 0.1;
		SVM_params.gamma = 1;
		SVM_params.coef0 = 0.1;
		SVM_params.C = 1;
		SVM_params.nu = 0.1;
		SVM_params.p = 0.1;
		SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 0.0001);

		//Train SVM
		cout << "Begin to generate svm" << endl;

        try {
            //CvSVM svm(trainingData, classes, Mat(), Mat(), SVM_params);
            svm.train_auto(trainingData, classes, Mat(), Mat(), SVM_params, 10,
                           CvSVM::get_default_grid(CvSVM::C),
                           CvSVM::get_default_grid(CvSVM::GAMMA),
                           CvSVM::get_default_grid(CvSVM::P),
                           CvSVM::get_default_grid(CvSVM::NU),
                           CvSVM::get_default_grid(CvSVM::COEF),
                           CvSVM::get_default_grid(CvSVM::DEGREE),
                           true);
        } catch (const Exception &err) {
            cout << err.what() << endl;
        }

		cout << "Svm generate done!" << endl;

		FileStorage fsTo("train/svm.xml", cv::FileStorage::WRITE);
		svm.write(*fsTo, "svm");
	}
	else
	{
        try {
            string path = "train/svm.xml";
            svm.load(path.c_str(), "svm");
        } catch (const Exception &err) {
            cout << err.what() << endl;
            return 0; //next predict requires svm
        }
	}

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

		//���ûص�������������
		Mat features;
		getFeatures(p, features);

		features = features.reshape(1, 1);
		features.convertTo(features, CV_32FC1);

		int predict = (int)svm.predict(features);
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
	cout << "ptrue_rtrue: " << ptrue_rtrue  << endl;
	cout << "ptrue_rfalse: " << ptrue_rfalse << endl;
	cout << "pfalse_rtrue: " << pfalse_rtrue  << endl;
	cout << "pfalse_rfalse: " << pfalse_rfalse  << endl;

	double precise = 0;
	if (ptrue_rtrue + ptrue_rfalse != 0)
	{
		precise = ptrue_rtrue / (ptrue_rtrue + ptrue_rfalse);
		cout << "precise: " << precise << endl;
	}
	else
		cout << "precise: " << "NA" << endl;

	double recall = 0;
	if (ptrue_rtrue + pfalse_rtrue != 0)
	{
		recall = ptrue_rtrue / (ptrue_rtrue + pfalse_rtrue);
		cout << "recall: " << recall << endl;
	}
	else
		cout << "recall: " << "NA" << endl;

	double Fsocre = 0;
	if (precise + recall != 0)
	{
		Fsocre = 2 * (precise * recall) / (precise + recall);
		cout << "Fsocre: " << Fsocre << endl;
	}
	else
		cout << "Fsocre: " << "NA" << endl;

	return 0;
}

}	/*! \namespace easypr*/
