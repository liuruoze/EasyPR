#include "easypr/svm_train.h"
#include "easypr/util.h"

namespace easypr {

Svm::Svm(const char* forward_data_folder, const char* inverse_data_folder)
        : forward_(forward_data_folder), inverse_(inverse_data_folder) {
  assert(forward_);
  assert(inverse_);
}

void Svm::divide(const char* images_folder, float percentage /* = 0.7 */) {
  auto files = Utils::getFiles(images_folder);
  if (files.empty()) {
    std::cout << "No file found in " << images_folder << std::endl;
    return;
  }
  assert(!files.empty());

  srand(unsigned(time(NULL)));
  random_shuffle(files.begin(), files.end());

  size_t split_index = size_t(files.size() * percentage);
  for (size_t i = 0; i < files.size(); ++i) {
    // TODO: Move files directly to improve efficiency.
    auto f = files[i];
    auto file_name = Utils::getFileName(f, true).c_str();
    auto image = cv::imread(f);
    char save_to[255] = {0};
    assert(!image.empty());
    if (i < split_index) {
      sprintf(save_to, "%s/train/%s", images_folder, file_name);
    } else {
      sprintf(save_to, "%s/test/%s", images_folder, file_name);
    }
    cv::imwrite(save_to, image);
    std::cout << f << " -> " << save_to << std::endl;
  }
}

//! 测试SVM的准确率，回归率以及FScore
//void Svm::accuracy(cv::Mat& testingclasses_preditc,
//                   cv::Mat& testingclasses_real) {
//  int channels = testingclasses_preditc.channels();
//  std::cout << "channels: " << channels << std::endl;
//  int nRows = testingclasses_preditc.rows;
//  std::cout << "nRows: " << nRows << std::endl;
//  int nCols = testingclasses_preditc.cols * channels;
//  std::cout << "nCols: " << nCols << std::endl;
//
//  int channels_real = testingclasses_real.channels();
//  std::cout << "channels_real: " << channels_real << std::endl;
//  int nRows_real = testingclasses_real.rows;
//  std::cout << "nRows_real: " << nRows_real << std::endl;
//  int nCols_real = testingclasses_real.cols * channels;
//  std::cout << "nCols_real: " << nCols_real << std::endl;
//
//  double count_all = 0;
//  double ptrue_rtrue = 0;
//  double ptrue_rfalse = 0;
//  double pfalse_rtrue = 0;
//  double pfalse_rfalse = 0;
//
//  for (int i = 0; i < nRows; i++) {
//    const uchar* inData = testingclasses_preditc.ptr<uchar>(i);
//    const uchar* outData = testingclasses_real.ptr<uchar>(i);
//
//    float predict = inData[0];
//    float real = outData[0];
//
//    count_all++;
//
//    if (predict == 1.0 && real == 1.0)
//      ptrue_rtrue++;
//    if (predict == 1.0 && real == 0)
//      ptrue_rfalse++;
//    if (predict == 0 && real == 1.0)
//      pfalse_rtrue++;
//    if (predict == 0 && real == 0)
//      pfalse_rfalse++;
//  }
//
//  std::cout << "count_all: " << count_all << std::endl;
//  std::cout << "ptrue_rtrue: " << ptrue_rtrue << std::endl;
//  std::cout << "ptrue_rfalse: " << ptrue_rfalse << std::endl;
//  std::cout << "pfalse_rtrue: " << pfalse_rtrue << std::endl;
//  std::cout << "pfalse_rfalse: " << pfalse_rfalse << std::endl;
//
//  double precise = 0;
//  if (ptrue_rtrue + ptrue_rfalse != 0) {
//    precise = ptrue_rtrue / (ptrue_rtrue + ptrue_rfalse);
//    std::cout << "precise: " << precise << std::endl;
//  }
//  else {
//    std::cout << "precise: " << "NA" << std::endl;
//  }
//
//  double recall = 0;
//  if (ptrue_rtrue + pfalse_rtrue != 0) {
//    recall = ptrue_rtrue / (ptrue_rtrue + pfalse_rtrue);
//    std::cout << "recall: " << recall << std::endl;
//  }
//  else {
//    std::cout << "recall: " << "NA" << std::endl;
//  }
//
//  double F = 0;
//  if (precise + recall != 0) {
//    F = (precise * recall) / (precise + recall);
//    std::cout << "F: " << F << std::endl;
//  }
//  else {
//    std::cout << "F: " << "NA" << std::endl;
//  }
//}
void Svm::get_train() {
  std::cout << "Training learn data..." << std::endl;

  std::vector<int> labels;
  auto folder = std::string(forward_).append("/train");

  // iterate forward data

  auto files = Utils::getFiles(folder);

  if (files.empty()) {
    std::cout << "No file found in " << folder << std::endl;
    return;
  }

  std::cout << "Training data in " << folder << std::endl;
  for (auto f : files) {
    auto image = cv::imread(f);

    cv::Mat features;
    getHisteqFeatures(image, features);
    features = features.reshape(1, 1);

    this->trainingData_.push_back(features);
    labels.push_back(Label::kForward); // Note here
  }

  // iterate inverse data

  files.clear();
  folder = std::string(inverse_).append("/train");
  files = Utils::getFiles(folder);

  if (files.empty()) {
    std::cout << "No file found in " << folder << std::endl;
    return;
  }

  std::cout << "Training data in " << folder << std::endl;
  for (auto f : files) {
    auto image = cv::imread(f);

    cv::Mat features;
    getHisteqFeatures(image, features);
    features = features.reshape(1, 1);

    this->trainingData_.push_back(features);
    labels.push_back(Label::kInverse); // Note here
  }

  this->trainingData_.convertTo(this->trainingData_, CV_32FC1);
  cv::Mat(labels).copyTo(this->classes_);
}

void Svm::get_test() {
  std::cout << "Tesing data..." << std::endl;

  auto folder = std::string(forward_).append("/test");
  auto files = Utils::getFiles(folder);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "No file found in " << folder << std::endl;
    return;
  }
  std::cout << "Testing in " << folder << std::endl;
  for (auto f : files) {
    auto img = cv::imread(f);
    test_imgaes_.push_back(img);
    test_labels_.push_back(kForward);
  }

  //

  folder.clear();
  folder = std::string(inverse_).append("/test");
  files = Utils::getFiles(folder);

  size = files.size();
  if (0 == size) {
    std::cout << "No file found in " << folder << std::endl;
    return;
  }
  std::cout << "Testing in " << folder << std::endl;
  for (auto f : files) {
    auto img = cv::imread(f);
    test_imgaes_.push_back(img);
    test_labels_.push_back(kInverse);
  }
}

void Svm::train(bool divide /* = true */, bool train /* = true */,
                float divide_percentage /* = 0.7 */,
                svmCallback getFeatures /* = getHistogramFeatures */) {
  if (divide) {
    std::cout << "Dividing learn data to train and test..." << std::endl;
    this->divide(forward_, divide_percentage);
    this->divide(inverse_, divide_percentage);
  }

  CvSVM svm;

  // 70% training procedure
  if (train) {
    this->get_train();

    if (!this->classes_.empty() && !this->trainingData_.empty()) {
      // need to be trained first
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

      std::cout << "Begin to generate svm" << std::endl;

      try {
        //CvSVM svm(trainingData, classes, cv::Mat(), cv::Mat(), SVM_params);
        svm.train_auto(this->trainingData_, this->classes_, cv::Mat(),
                       cv::Mat(),
                       SVM_params,
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
      // don't train, use ready-made model file
      try {
        svm.load("resources/train/svm.xml", "svm");
      } catch (const cv::Exception& err) {
        std::cout << err.what() << std::endl;
      }
    }
  } // if train

  // 30% testing procedure
  this->get_test();

  std::cout << "Begin to predict" << std::endl;

  double count_all = test_imgaes_.size();
  double ptrue_rtrue = 0;
  double ptrue_rfalse = 0;
  double pfalse_rtrue = 0;
  double pfalse_rfalse = 0;

  size_t label_index = 0;
  for (auto image : test_imgaes_) {
    //调用回调函数决定特征
    cv::Mat features;
    getHistogramFeatures(image, features);

    features = features.reshape(1, 1);
    features.convertTo(features, CV_32FC1);

    Label predict = ((int) svm.predict(features)) == 1 ? kForward : kInverse;
    Label real = test_labels_[label_index++];

    if (predict == kForward && real == kForward)
      ptrue_rtrue++;
    if (predict == kForward && real == kInverse)
      ptrue_rfalse++;
    if (predict == kInverse && real == kForward)
      pfalse_rtrue++;
    if (predict == kInverse && real == kInverse)
      pfalse_rfalse++;
  }

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
  else {
    std::cout << "Fsocre: " << "NA" << std::endl;
  }

}

}
