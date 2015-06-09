// ann_train.cpp : ann模型的训练文件，主要用在OCR中

#include <opencv2/opencv.hpp>
#include "easypr/plate_recognize.h"
#include "easypr/util.h"

#ifdef OS_WINDOWS
#include <windows.h>
#endif

#include <vector>
#include <iostream>
#include <cstdlib>

#ifdef OS_WINDOWS
#include <io.h>
#endif

using namespace easypr;
using namespace cv;
using namespace std;

#define HORIZONTAL 1
#define VERTICAL 0

CvANN_MLP ann;

//中国车牌
const char strCharacters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F', 'G',
                              'H', /* 没有I */
                              'J', 'K', 'L', 'M', 'N', /* 没有O */ 'P', 'Q',
                              'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const int numCharacter = 34;
/* 没有I和O,10个数字与24个英文字符之和 */

//以下都是我训练时用到的中文字符数据，并不全面，有些省份没有训练数据所以没有字符
//有些后面加数字2的表示在训练时常看到字符的一种变形，也作为训练数据存储
const std::string strChinese[] = {
    "zh_cuan" /* 川 */,  "zh_e" /* 鄂 */,    "zh_gan" /* 赣*/,
    "zh_gan1" /*甘*/,    "zh_gui" /* 贵 */,  "zh_gui1" /* 桂 */,
    "zh_hei" /* 黑 */,   "zh_hu" /* 沪 */,   "zh_ji" /* 冀 */,
    "zh_jin" /* 津 */,   "zh_jing" /* 京 */, "zh_jl" /* 吉 */,
    "zh_liao" /* 辽 */,  "zh_lu" /* 鲁 */,   "zh_meng" /* 蒙 */,
    "zh_min" /* 闽 */,   "zh_ning" /* 宁 */, "zh_qing" /* 青 */,
    "zh_qiong" /* 琼 */, "zh_shan" /* 陕 */, "zh_su" /* 苏 */,
    "zh_sx" /* 晋 */,    "zh_wan" /* 皖 */,  "zh_xiang" /* 湘 */,
    "zh_xin" /* 新 */,   "zh_yu" /* 豫 */,   "zh_yu1" /* 渝 */,
    "zh_yue" /* 粤 */,   "zh_yun" /* 云 */,  "zh_zang" /* 藏 */,
    "zh_zhe" /* 浙 */};

const int numChinese = 31;
const int numAll = 65;

/* 34+20=54 */

void annTrain(Mat TrainData, Mat classes, int nNeruns) {
  ann.clear();
  Mat layers(1, 3, CV_32SC1);
  layers.at<int>(0) = TrainData.cols;
  layers.at<int>(1) = nNeruns;
  layers.at<int>(2) = numAll;
  ann.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

  // Prepare trainClases
  // Create a mat with n trained data by m classes
  Mat trainClasses;
  trainClasses.create(TrainData.rows, numAll, CV_32FC1);
  for (int i = 0; i < trainClasses.rows; i++) {
    for (int k = 0; k < trainClasses.cols; k++) {
      // If class of data i is same than a k class
      if (k == classes.at<int>(i))
        trainClasses.at<float>(i, k) = 1;
      else
        trainClasses.at<float>(i, k) = 0;
    }
  }
  Mat weights(1, TrainData.rows, CV_32FC1, Scalar::all(1));

  // Learn classifier
  // ann.train( TrainData, trainClasses, weights );

  // Setup the BPNetwork

  // Set up BPNetwork's parameters
  CvANN_MLP_TrainParams params;
  params.train_method = CvANN_MLP_TrainParams::BACKPROP;
  params.bp_dw_scale = 0.1;
  params.bp_moment_scale = 0.1;

  // params.train_method=CvANN_MLP_TrainParams::RPROP;
  // params.rp_dw0 = 0.1;
  // params.rp_dw_plus = 1.2;
  // params.rp_dw_minus = 0.5;
  // params.rp_dw_min = FLT_EPSILON;
  // params.rp_dw_max = 50.;

  ann.train(TrainData, trainClasses, Mat(), Mat(), params);
}

int saveTrainData() {
  cout << "Begin saveTrainData" << endl;
  Mat classes;
  Mat trainingDataf5;
  Mat trainingDataf10;
  Mat trainingDataf15;
  Mat trainingDataf20;

  vector<int> trainingLabels;
  string path = "resources/train/data/chars_recognise_ann/chars2";

  for (int i = 0; i < numCharacter; i++) {
    cout << "Character: " << strCharacters[i] << "\n";
    stringstream ss(stringstream::in | stringstream::out);
    ss << path << "/" << strCharacters[i];

    auto files = Utils::getFiles(ss.str());

    size_t size = files.size();
    for (int j = 0; j < size; j++) {
      cout << files[j].c_str() << endl;
      Mat img = imread(files[j].c_str(), 0);
      Mat f5 = features(img, 5);
      Mat f10 = features(img, 10);
      Mat f15 = features(img, 15);
      Mat f20 = features(img, 20);

      trainingDataf5.push_back(f5);
      trainingDataf10.push_back(f10);
      trainingDataf15.push_back(f15);
      trainingDataf20.push_back(f20);
      trainingLabels.push_back(i);  //每一幅字符图片所对应的字符类别索引下标
    }
  }

  path = "resources/train/data/chars_recognise_ann/charsChinese";

  for (int i = 0; i < numChinese; i++) {
    cout << "Character: " << strChinese[i] << "\n";
    stringstream ss(stringstream::in | stringstream::out);
    ss << path << "/" << strChinese[i];

    auto files = Utils::getFiles(ss.str());

    int size = files.size();
    for (int j = 0; j < size; j++) {
      cout << files[j].c_str() << endl;
      Mat img = imread(files[j].c_str(), 0);
      Mat f5 = features(img, 5);
      Mat f10 = features(img, 10);
      Mat f15 = features(img, 15);
      Mat f20 = features(img, 20);

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

  FileStorage fs("resources/train/ann_data.xml", FileStorage::WRITE);
  fs << "TrainingDataF5" << trainingDataf5;
  fs << "TrainingDataF10" << trainingDataf10;
  fs << "TrainingDataF15" << trainingDataf15;
  fs << "TrainingDataF20" << trainingDataf20;
  fs << "classes" << classes;
  fs.release();

  cout << "End saveTrainData" << endl;

  return 0;
}

void saveModel(int _predictsize, int _neurons) {
  FileStorage fs;
  fs.open("resources/train/ann_data.xml", FileStorage::READ);

  Mat TrainingData;
  Mat Classes;

  string training;
  if (1) {
    stringstream ss(stringstream::in | stringstream::out);
    ss << "TrainingDataF" << _predictsize;
    training = ss.str();
  }

  fs[training] >> TrainingData;
  fs["classes"] >> Classes;

  // train the Ann
  cout << "Begin to saveModelChar predictSize:" << _predictsize
       << " neurons:" << _neurons << endl;

  long start = Utils::getTimestamp();
  annTrain(TrainingData, Classes, _neurons);

  long end = Utils::getTimestamp();
  cout << "Elapse:" << (end - start) / 1000 << endl;

  cout << "End the saveModelChar" << endl;

  string model_name = "resources/train/ann.xml";
  // if(1)
  //{
  //	stringstream ss(stringstream::in | stringstream::out);
  //	ss << "ann_prd" << _predictsize << "_neu"<< _neurons << ".xml";
  //	model_name = ss.str();
  //}

  FileStorage fsTo(model_name, cv::FileStorage::WRITE);
  ann.write(*fsTo, "ann");
}

int annMain() {
  cout << "To be begin." << endl;

  saveTrainData();

  //可根据需要训练不同的predictSize或者neurons的ANN模型
  // for (int i = 2; i <= 2; i ++)
  //{
  //	int size = i * 5;
  //	for (int j = 5; j <= 10; j++)
  //	{
  //		int neurons = j * 10;
  //		saveModel(size, neurons);
  //	}
  //}

  //这里演示只训练model文件夹下的ann.xml，此模型是一个predictSize=10,neurons=40的ANN模型。
  //根据机器的不同，训练时间不一样，但一般需要10分钟左右，所以慢慢等一会吧。
  saveModel(10, 40);

  cout << "To be end." << endl;
  int end;
  cin >> end;
  return 0;
}