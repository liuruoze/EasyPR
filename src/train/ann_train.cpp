// ann_train.cpp : annģ�͵�ѵ���ļ�����Ҫ����OCR��

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"

#if defined (WIN32) || defined (_WIN32)
#include <windows.h>
#endif

#include <vector>
#include <iostream>
#include <cstdlib>

#if defined (WIN32) || defined (_WIN32)
#include <io.h>
#elif defined (linux) || defined (__linux__)
//#include <sys/io.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "../include/plate_recognize.h"
#include "../include/feature.h"
#include "../include/util.h"

using namespace easypr;

#define HORIZONTAL    1
#define VERTICAL    0

CvANN_MLP  ann;

//�й�����
const char strCharacters[] = {'0','1','2','3','4','5',\
    '6','7','8','9','A','B', 'C', 'D', 'E','F', 'G', 'H', /* û��I */\
    'J', 'K', 'L', 'M', 'N', /* û��O */ 'P', 'Q', 'R', 'S', 'T', \
    'U','V', 'W', 'X', 'Y', 'Z'};
const int numCharacter = 34; /* û��I��O,10��������24��Ӣ���ַ�֮�� */

//���¶�����ѵ��ʱ�õ��������ַ����ݣ�����ȫ�棬��Щʡ��û��ѵ����������û���ַ�
//��Щ���������2�ı�ʾ��ѵ��ʱ�������ַ���һ�ֱ��Σ�Ҳ��Ϊѵ�����ݴ洢
const string strChinese[] = {"zh_cuan" /* �� */, "zh_e" /* �� */,  "zh_gan" /* ��*/, \
    "zh_hei" /* �� */, "zh_hu" /* �� */,  "zh_ji" /* �� */, \
    "zh_jl" /* �� */, "zh_jin" /* �� */, "zh_jing" /* �� */, "zh_shan" /* �� */, \
    "zh_liao" /* �� */, "zh_lu" /* ³ */, "zh_min" /* �� */, "zh_ning" /* �� */, \
    "zh_su" /* �� */,  "zh_sx" /* �� */, "zh_wan" /* �� */,\
    "zh_yu" /* ԥ */, "zh_yue" /* �� */, "zh_zhe" /* �� */};

const int numChinese = 20;
const int numAll = 54; /* 34+20=54 */

const int numNeurons = 20;
const int predictSize = 10;

Mat features(Mat in, int sizeData){
    //Histogram features
    Mat vhist=ProjectedHistogram(in,VERTICAL);
    Mat hhist=ProjectedHistogram(in,HORIZONTAL);

    //Low data feature
    Mat lowData;
    resize(in, lowData, Size(sizeData, sizeData) );

    //Last 10 is the number of moments components
    int numCols=vhist.cols+hhist.cols+lowData.cols*lowData.cols;

    Mat out=Mat::zeros(1,numCols,CV_32F);
    //Asign values to feature,ANN����������Ϊˮƽ����ֱֱ��ͼ�͵ͷֱ���ͼ������ɵ�ʸ��
    int j=0;
    for(int i=0; i<vhist.cols; i++)
    {
        out.at<float>(j)=vhist.at<float>(i);
        j++;
    }
    for(int i=0; i<hhist.cols; i++)
    {
        out.at<float>(j)=hhist.at<float>(i);
        j++;
    }
    for(int x=0; x<lowData.cols; x++)
    {
        for(int y=0; y<lowData.rows; y++){
            out.at<float>(j)=(float)lowData.at<unsigned char>(x,y);
            j++;
        }
    }
    //if(DEBUG)
    //	cout << out << "\n===========================================\n";
    return out;
}

void annTrain(Mat TrainData, Mat classes, int nNeruns)
{
    ann.clear();
    Mat layers(1, 3, CV_32SC1);
    layers.at<int>(0) = TrainData.cols;
    layers.at<int>(1) = nNeruns;
    layers.at<int>(2) = numAll;
    ann.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

    //Prepare trainClases
    //Create a mat with n trained data by m classes
    Mat trainClasses;
    trainClasses.create( TrainData.rows, numAll, CV_32FC1 );
    for( int i = 0; i <  trainClasses.rows; i++ )
    {
        for( int k = 0; k < trainClasses.cols; k++ )
        {
            //If class of data i is same than a k class
            if( k == classes.at<int>(i) )
                trainClasses.at<float>(i,k) = 1;
            else
                trainClasses.at<float>(i,k) = 0;
        }
    }
    Mat weights( 1, TrainData.rows, CV_32FC1, Scalar::all(1) );

    //Learn classifier
    ann.train( TrainData, trainClasses, weights );
}

int saveTrainData()
{
    cout << "Begin saveTrainData" << endl;
    Mat classes;
    Mat trainingDataf5;
    Mat trainingDataf10;
    Mat trainingDataf15;
    Mat trainingDataf20;

    vector<int> trainingLabels;
    string path = "train/data/chars_recognise_ann/chars2/chars2";

    for(int i = 0; i < numCharacter; i++)
    {
        cout << "Character: "<< strCharacters[i] << "\n";
        stringstream ss(stringstream::in | stringstream::out);
        ss << path << "/" << strCharacters[i];

        auto files = Utils::getFiles(ss.str());

        int size = files.size();
        for (int j = 0; j < size; j++)
        {
            cout << files[j].c_str() << endl;
            Mat img = imread(files[j].c_str(), 0);
            Mat f5=features(img, 5);
            Mat f10=features(img, 10);
            Mat f15=features(img, 15);
            Mat f20=features(img, 20);

            trainingDataf5.push_back(f5);
            trainingDataf10.push_back(f10);
            trainingDataf15.push_back(f15);
            trainingDataf20.push_back(f20);
            trainingLabels.push_back(i);			//ÿһ���ַ�ͼƬ����Ӧ���ַ���������±�
        }
    }

    path = "train/data/chars_recognise_ann/charsChinese/charsChinese";

    for (int i = 0; i < numChinese; i++)
    {
        cout << "Character: "<< strChinese[i] << "\n";
        stringstream ss(stringstream::in | stringstream::out);
        ss << path << "/" << strChinese[i];

        auto files = Utils::getFiles(ss.str());

        int size = files.size();
        for (int j = 0; j < size; j++)
        {
            cout << files[j].c_str() << endl;
            Mat img = imread(files[j].c_str(), 0);
            Mat f5=features(img, 5);
            Mat f10=features(img, 10);
            Mat f15=features(img, 15);
            Mat f20=features(img, 20);

            trainingDataf5.push_back(f5);
            trainingDataf10.push_back(f10);
            trainingDataf15.push_back(f15);
            trainingDataf20.push_back(f20);
            trainingLabels.push_back(i + numCharacter);
        }
    }

    trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
    trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
    trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
    trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
    Mat(trainingLabels).copyTo(classes);

    FileStorage fs("train/ann_data.xml", FileStorage::WRITE);
    fs << "TrainingDataF5" << trainingDataf5;
    fs << "TrainingDataF10" << trainingDataf10;
    fs << "TrainingDataF15" << trainingDataf15;
    fs << "TrainingDataF20" << trainingDataf20;
    fs << "classes" << classes;
    fs.release();

    cout << "End saveTrainData" << endl;

    return 0;
}

void saveModel(int _predictsize, int _neurons)
{
    FileStorage fs;
    fs.open("train/ann_data.xml", FileStorage::READ);

    Mat TrainingData;
    Mat Classes;

    string training;
    if(1)
    {
        stringstream ss(stringstream::in | stringstream::out);
        ss << "TrainingDataF" << _predictsize;
        training = ss.str();
    }

    fs[training] >> TrainingData;
    fs["classes"] >> Classes;

    //train the Ann
    cout << "Begin to saveModelChar predictSize:" << _predictsize
    << " neurons:" << _neurons << endl;

    long start = Utils::getTimestamp();
    annTrain(TrainingData, Classes, _neurons);
    long end = Utils::getTimestamp();
    cout << "Elapse:" << (end-start)/1000 << endl;

    cout << "End the saveModelChar" << endl;

    string model_name = "train/ann.xml";
    //if(1)
    //{
    //	stringstream ss(stringstream::in | stringstream::out);
    //	ss << "ann_prd" << _predictsize << "_neu"<< _neurons << ".xml";
    //	model_name = ss.str();
    //}

    FileStorage fsTo(model_name, cv::FileStorage::WRITE);
    ann.write(*fsTo, "ann");
}

int annMain()
{
    cout << "To be begin." << endl;

    saveTrainData();

    //�ɸ�����Ҫѵ����ͬ��predictSize����neurons��ANNģ��
    //for (int i = 2; i <= 2; i ++)
    //{
    //	int size = i * 5;
    //	for (int j = 5; j <= 10; j++)
    //	{
    //		int neurons = j * 10;
    //		saveModel(size, neurons);
    //	}
    //}

    //������ʾֻѵ��model�ļ����µ�ann.xml����ģ����һ��predictSize=10,neurons=40��ANNģ�͡�
    //���ݻ����Ĳ�ͬ��ѵ��ʱ�䲻һ������һ����Ҫ10�������ң�����������һ��ɡ�
    saveModel(10, 40);

    cout << "To be end." << endl;
    int end;
    cin >> end;
    return 0;
}
