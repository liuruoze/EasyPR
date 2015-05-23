#include "easypr/svm_train.h"
#include "easypr/util.h"

namespace easypr {

void Svm::learn2HasPlate(float bound /* = 0.7 */) {

  const char* filePath = "resources/train/data/plate_detect_svm/learn/HasPlate";

  ////获取该路径下的所有文件
  auto files = Utils::getFiles(filePath);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "File not found in " << filePath << std::endl;
    return;
  }
  ////随机选取70%作为训练数据，30%作为测试数据
  srand(unsigned(time(NULL)));
  random_shuffle(files.begin(), files.end());

  size_t boundry = (size_t) bound * size;

  std::cout << "Save HasPlate train!" << std::endl;
  for (int i = 0; i < boundry; i++) {
    std::cout << files[i].c_str() << std::endl;
    cv::Mat img = cv::imread(files[i].c_str());
    // if (1) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << "resources/train/data/plate_detect_svm/train/HasPlate/hasplate_" <<
    i <<
    ".jpg";
    imwrite(ss.str(), img);
    // }
  }

  std::cout << "Save HasPlate test!" << std::endl;
  for (size_t i = boundry; i < size; i++) {
    std::cout << files[i].c_str() << std::endl;
    cv::Mat img = cv::imread(files[i].c_str());
    // if (1) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << "resources/train/data/plate_detect_svm/test/HasPlate/hasplate_" <<
    i <<
    ".jpg";
    imwrite(ss.str(), img);
    // }
  }
}

void Svm::learn2NoPlate(float bound /* = 0.7 */) {

  const char* filePath = "resources/train/data/plate_detect_svm/learn/NoPlate";

  ////获取该路径下的所有文件
  auto files = Utils::getFiles(filePath);
  size_t size = files.size();
  if (0 == size) {
    std::cout << "File not found in " << filePath << std::endl;
    return;
  }
  ////随机选取70%作为训练数据，30%作为测试数据
  srand(unsigned(time(NULL)));
  random_shuffle(files.begin(), files.end());

  size_t boundry = (size_t) bound * size;

  std::cout << "Save NoPlate train!" << std::endl;
  for (int i = 0; i < boundry; i++) {
    std::cout << files[i].c_str() << std::endl;
    cv::Mat img = cv::imread(files[i].c_str());
    if (1) {
      std::stringstream ss(std::stringstream::in | std::stringstream::out);
      ss << "resources/train/data/plate_detect_svm/train/NoPlate/noplate_" <<
      i << ".jpg";
      imwrite(ss.str(), img);
    }
  }

  std::cout << "Save NoPlate test!" << std::endl;
  for (size_t i = boundry; i < size; i++) {
    std::cout << files[i].c_str() << std::endl;
    cv::Mat img = cv::imread(files[i].c_str());
    if (1) {
      std::stringstream ss(std::stringstream::in | std::stringstream::out);
      ss << "resources/train/data/plate_detect_svm/test/NoPlate/noplate_" <<
      i << ".jpg";
      imwrite(ss.str(), img);
    }
  }
}

void Svm::getHasPlateTrain(cv::Mat& trainingImages,
                           std::vector<int>& trainingLabels,
                           svmCallback getFeatures /* = getHisteqFeatures */) {
  int label = 1;
  const char* filePath = "resources/train/data/plate_detect_svm/train/HasPlate";

  ////获取该路径下的所有文件
  auto files = Utils::getFiles(filePath);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "File not found in " << filePath << std::endl;
    return;
  }
  std::cout << "get HasPlate train!" << std::endl;
  for (int i = 0; i < size; i++) {
    cv::Mat img = cv::imread(files[i].c_str());

    //调用回调函数决定特征
    cv::Mat features;
    getFeatures(img, features);
    features = features.reshape(1, 1);

    trainingImages.push_back(features);
    trainingLabels.push_back(label);
  }
}

void Svm::getNoPlateTrain(cv::Mat& trainingImages,
                          std::vector<int>& trainingLabels,
                          svmCallback getFeatures /* = getHisteqFeatures */) {
  int label = 0;
  const char* filePath = "resources/train/data/plate_detect_svm/train/NoPlate";

  ////获取该路径下的所有文件
  auto files = Utils::getFiles(filePath);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "File not found in " << filePath << std::endl;
    return;
  }
  std::cout << "get NoPlate train!" << std::endl;
  for (int i = 0; i < size; i++) {
    cv::Mat img = cv::imread(files[i].c_str());

    //调用回调函数决定特征
    cv::Mat features;
    getFeatures(img, features);
    features = features.reshape(1, 1);

    trainingImages.push_back(features);
    trainingLabels.push_back(label);
  }
}

void Svm::getHasPlateTest(std::vector<cv::Mat>& testingImages,
                          std::vector<int>& testingLabels) {
  int label = 1;
  const char* filePath = "resources/train/data/plate_detect_svm/test/HasPlate";

  ////获取该路径下的所有文件
  auto files = Utils::getFiles(filePath);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "File not found in " << filePath << std::endl;
    return;
  }
  std::cout << "get HasPlate test!" << std::endl;
  for (int i = 0; i < size; i++) {
    cv::Mat img = cv::imread(files[i].c_str());

    testingImages.push_back(img);
    testingLabels.push_back(label);
  }
}

void Svm::getNoPlateTest(std::vector<cv::Mat>& testingImages,
                         std::vector<int>& testingLabels) {
  int label = 0;
  const char* filePath = "resources/train/data/plate_detect_svm/test/NoPlate";

  ////获取该路径下的所有文件
  auto files = Utils::getFiles(filePath);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "File not found in " << filePath << std::endl;
    return;
  }
  std::cout << "get NoPlate test!" << std::endl;
  for (int i = 0; i < size; i++) {
    //std::cout << files[i].c_str() << std::endl;
    cv::Mat img = cv::imread(files[i].c_str());

    testingImages.push_back(img);
    testingLabels.push_back(label);
  }
}


//! 测试SVM的准确率，回归率以及FScore
void Svm::accuracy(cv::Mat& testingclasses_preditc,
                   cv::Mat& testingclasses_real) {
  int channels = testingclasses_preditc.channels();
  std::cout << "channels: " << channels << std::endl;
  int nRows = testingclasses_preditc.rows;
  std::cout << "nRows: " << nRows << std::endl;
  int nCols = testingclasses_preditc.cols * channels;
  std::cout << "nCols: " << nCols << std::endl;

  int channels_real = testingclasses_real.channels();
  std::cout << "channels_real: " << channels_real << std::endl;
  int nRows_real = testingclasses_real.rows;
  std::cout << "nRows_real: " << nRows_real << std::endl;
  int nCols_real = testingclasses_real.cols * channels;
  std::cout << "nCols_real: " << nCols_real << std::endl;

  double count_all = 0;
  double ptrue_rtrue = 0;
  double ptrue_rfalse = 0;
  double pfalse_rtrue = 0;
  double pfalse_rfalse = 0;

  for (int i = 0; i < nRows; i++) {
    const uchar* inData = testingclasses_preditc.ptr<uchar>(i);
    const uchar* outData = testingclasses_real.ptr<uchar>(i);

    float predict = inData[0];
    float real = outData[0];

    count_all++;

    if (predict == 1.0 && real == 1.0)
      ptrue_rtrue++;
    if (predict == 1.0 && real == 0)
      ptrue_rfalse++;
    if (predict == 0 && real == 1.0)
      pfalse_rtrue++;
    if (predict == 0 && real == 0)
      pfalse_rfalse++;
  }

  std::cout << "count_all: " << count_all << std::endl;
  std::cout << "ptrue_rtrue: " << ptrue_rtrue << std::endl;
  std::cout << "ptrue_rfalse: " << ptrue_rfalse << std::endl;
  std::cout << "pfalse_rtrue: " << pfalse_rtrue << std::endl;
  std::cout << "pfalse_rfalse: " << pfalse_rfalse << std::endl;

  double precise = 0;
  if (ptrue_rtrue + ptrue_rfalse != 0) {
    precise = ptrue_rtrue / (ptrue_rtrue + ptrue_rfalse);
    std::cout << "precise: " << precise << std::endl;
  }
  else {
    std::cout << "precise: " << "NA" << std::endl;
  }

  double recall = 0;
  if (ptrue_rtrue + pfalse_rtrue != 0) {
    recall = ptrue_rtrue / (ptrue_rtrue + pfalse_rtrue);
    std::cout << "recall: " << recall << std::endl;
  }
  else {
    std::cout << "recall: " << "NA" << std::endl;
  }

  double F = 0;
  if (precise + recall != 0) {
    F = (precise * recall) / (precise + recall);
    std::cout << "F: " << F << std::endl;
  }
  else {
    std::cout << "F: " << "NA" << std::endl;
  }
}


int Svm::train(bool dividePrepared /* = true */,
               bool trainPrepared /* = true */,
               svmCallback getFeatures /* = getHistogramFeatures */) {
  cv::Mat classes;
  cv::Mat trainingData;

  cv::Mat trainingImages;
  std::vector<int> trainingLabels;

  if (!dividePrepared) {
    //分割learn里的数据到train和test里
    std::cout << "Divide learn to train and test" << std::endl;
    learn2HasPlate();
    learn2NoPlate();
  }

  //将训练数据加载入内存
  if (!trainPrepared) {
    std::cout << "Begin to get train data to memory" << std::endl;
    getHasPlateTrain(trainingImages, trainingLabels, getFeatures);
    getNoPlateTrain(trainingImages, trainingLabels, getFeatures);

    cv::Mat(trainingImages).copyTo(trainingData);
    trainingData.convertTo(trainingData, CV_32FC1);
    cv::Mat(trainingLabels).copyTo(classes);
  }

  //Test SVM
  std::vector<cv::Mat> testingImages;
  std::vector<int> testingLabels_real;

  //将测试数据加载入内存
  std::cout << "Begin to get test data to memory" << std::endl;
  getHasPlateTest(testingImages, testingLabels_real);
  getNoPlateTest(testingImages, testingLabels_real);

  CvSVM svm;
  if (!trainPrepared && !classes.empty() && !trainingData.empty()) {
    CvSVMParams SVM_params;
    SVM_params.svm_type = CvSVM::C_SVC;
    //SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;   线型，也就是无核
    SVM_params.kernel_type = CvSVM::RBF; //CvSVM::RBF 径向基函数，也就是高斯核
    SVM_params.degree = 0.1;
    SVM_params.gamma = 1;
    SVM_params.coef0 = 0.1;
    SVM_params.C = 1;
    SVM_params.nu = 0.1;
    SVM_params.p = 0.1;
    SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 0.0001);

    //Train SVM
    std::cout << "Begin to generate svm" << std::endl;

    try {
      //CvSVM svm(trainingData, classes, cv::Mat(), cv::Mat(), SVM_params);
      svm.train_auto(trainingData, classes, cv::Mat(), cv::Mat(), SVM_params,
                     10,
                     CvSVM::get_default_grid(CvSVM::C),
                     CvSVM::get_default_grid(CvSVM::GAMMA),
                     CvSVM::get_default_grid(CvSVM::P),
                     CvSVM::get_default_grid(CvSVM::NU),
                     CvSVM::get_default_grid(CvSVM::COEF),
                     CvSVM::get_default_grid(CvSVM::DEGREE),
                     true);
    } catch (const cv::Exception& err) {
      std::cout << err.what() << std::endl;
    }

    std::cout << "Svm generate done!" << std::endl;

    cv::FileStorage fsTo("resources/train/svm.xml", cv::FileStorage::WRITE);
    svm.write(*fsTo, "svm");
  }
  else {
    try {
      svm.load("resources/train/svm.xml", "svm");
    } catch (const cv::Exception& err) {
      std::cout << err.what() << std::endl;
      return 0; //next predict requires svm
    }
  }

  std::cout << "Begin to predict" << std::endl;

  double count_all = 0;
  double ptrue_rtrue = 0;
  double ptrue_rfalse = 0;
  double pfalse_rtrue = 0;
  double pfalse_rfalse = 0;

  size_t size = testingImages.size();
  for (int i = 0; i < size; i++) {
    cv::Mat p = testingImages[i];

    //调用回调函数决定特征
    cv::Mat features;
    getFeatures(p, features);

    features = features.reshape(1, 1);
    features.convertTo(features, CV_32FC1);

    int predict = (int) svm.predict(features);
    int real = testingLabels_real[i];

    if (predict == 1 && real == 1)
      ptrue_rtrue++;
    if (predict == 1 && real == 0)
      ptrue_rfalse++;
    if (predict == 0 && real == 1)
      pfalse_rtrue++;
    if (predict == 0 && real == 0)
      pfalse_rfalse++;
  }

  count_all = double(size);

  std::cout << "Get the Accuracy!" << std::endl;

  std::cout << "count_all: " << count_all << std::endl;
  std::cout << "ptrue_rtrue: " << ptrue_rtrue << std::endl;
  std::cout << "ptrue_rfalse: " << ptrue_rfalse << std::endl;
  std::cout << "pfalse_rtrue: " << pfalse_rtrue << std::endl;
  std::cout << "pfalse_rfalse: " << pfalse_rfalse << std::endl;

  double precise = 0;
  if (ptrue_rtrue + ptrue_rfalse != 0) {
    precise = ptrue_rtrue / (ptrue_rtrue + ptrue_rfalse);
    std::cout << "precise: " << precise << std::endl;
  }
  else
    std::cout << "precise: " << "NA" << std::endl;

  double recall = 0;
  if (ptrue_rtrue + pfalse_rtrue != 0) {
    recall = ptrue_rtrue / (ptrue_rtrue + pfalse_rtrue);
    std::cout << "recall: " << recall << std::endl;
  }
  else
    std::cout << "recall: " << "NA" << std::endl;

  double Fsocre = 0;
  if (precise + recall != 0) {
    Fsocre = 2 * (precise * recall) / (precise + recall);
    std::cout << "Fsocre: " << Fsocre << std::endl;
  }
  else
    std::cout << "Fsocre: " << "NA" << std::endl;

  return 0;
}

}
