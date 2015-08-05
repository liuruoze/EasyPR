#include "easypr/train/svm_train.h"
#include <ctime>
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"

namespace easypr {

  SvmTrain::SvmTrain(const char* forward_data_folder, const char* inverse_data_folder)
    : forward_(forward_data_folder), inverse_(inverse_data_folder) {
    assert(forward_);
    assert(inverse_);
  }

  void SvmTrain::train(bool divide /* = true */, float divide_percentage /* = 0.7 */,
    const char* out_svm_path /* = kDefaultSvmPath */) {

    if (divide) {
      std::cout << "Dividing data to be trained and tested..." << std::endl;
      this->divide(forward_, divide_percentage);
      this->divide(inverse_, divide_percentage);
    }

    cv::Ptr<ml::SVM> svm;

    // 70% training procedure
    this->getTrain();

    svm = ml::SVM::create();

    if (!this->classes_.empty() && !this->trainingData_.empty()) {
      // need to be trained first
      svm->setType(ml::SVM::C_SVC);
      svm->setKernel(ml::SVM::RBF);
      svm->setDegree(0.1);
      svm->setGamma(1);
      svm->setCoef0(0.1);
      svm->setC(1);
      svm->setNu(0.1);
      svm->setP(0.1);
      svm->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 100000, 0.0001));

      std::cout << "Generating svm model file, please wait..." << std::endl;

      try {
        svm->trainAuto(this->trainingData_, 10,
          ml::SVM::getDefaultGrid(ml::SVM::C),
          ml::SVM::getDefaultGrid(ml::SVM::GAMMA),
          ml::SVM::getDefaultGrid(ml::SVM::P),
          ml::SVM::getDefaultGrid(ml::SVM::NU),
          ml::SVM::getDefaultGrid(ml::SVM::COEF),
          ml::SVM::getDefaultGrid(ml::SVM::DEGREE), false);
      }
      catch (const cv::Exception& err) {
        std::cout << err.what() << std::endl;
      }

      svm->save(out_svm_path);

      std::cout << "Generate done! The model file is located at "
        << out_svm_path << std::endl;
    }
  }

  void SvmTrain::runTest(const char* svm_path /* = kDefaultSvmPath */){
    // TODO Check whether the model file exists or not.
    auto svm = Algorithm::load<ml::SVM>(svm_path);

    this->getTest();

    std::cout << "Testing..." << std::endl;

    double count_all = test_imgaes_.size();
    double ptrue_rtrue = 0;
    double ptrue_rfalse = 0;
    double pfalse_rtrue = 0;
    double pfalse_rfalse = 0;

    size_t label_index = 0;
    for (auto image : test_imgaes_) {
      //调用回调函数决定特征
      auto features = easypr::histeq(image);
      features = features.reshape(1, 1);
      cv::Mat out;
      features.convertTo(out, CV_32FC1);

      Label predict = ((int)svm->predict(out)) == 1 ? kForward : kInverse;
      Label real = test_labels_[label_index++];

      if (predict == kForward && real == kForward) ptrue_rtrue++;
      if (predict == kForward && real == kInverse) ptrue_rfalse++;
      if (predict == kInverse && real == kForward) pfalse_rtrue++;
      if (predict == kInverse && real == kInverse) pfalse_rfalse++;
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
      std::cout << "precise: "
        << "NA" << std::endl;
    }

    double recall = 0;
    if (ptrue_rtrue + pfalse_rtrue != 0) {
      recall = ptrue_rtrue / (ptrue_rtrue + pfalse_rtrue);
      std::cout << "recall: " << recall << std::endl;
    }
    else {
      std::cout << "recall: "
        << "NA" << std::endl;
    }

    double Fsocre = 0;
    if (precise + recall != 0) {
      Fsocre = 2 * (precise * recall) / (precise + recall);
      std::cout << "Fsocre: " << Fsocre << std::endl;
    }
    else {
      std::cout << "Fsocre: "
        << "NA" << std::endl;
    }
  }

  void SvmTrain::divide(const char* images_folder, float percentage /* = 0.7 */) {
    auto files = utils::getFiles(images_folder);
    if (files.empty()) {
      std::cout << "No file found in " << images_folder << std::endl;
      return;
    }

    srand(unsigned(time(NULL)));
    random_shuffle(files.begin(), files.end());

    size_t split_index = size_t(files.size() * percentage);
    for (size_t i = 0; i < files.size(); ++i) {
      // TODO: Move files directly to improve efficiency.
      auto f = files[i];
      auto file_name = utils::getFileName(f, true);
      auto image = cv::imread(f);
      char save_to[255] = { 0 };
      assert(!image.empty());
      if (i < split_index) {
        sprintf(save_to, "%s/train/%s", images_folder, file_name.c_str());
      }
      else {
        sprintf(save_to, "%s/test/%s", images_folder, file_name.c_str());
      }
      utils::imwrite(save_to, image);
      std::cout << f << " -> " << save_to << std::endl;
    }
  }

  void SvmTrain::getTrain() {
    std::cout << "Collecting train data..." << std::endl;

    cv::Mat samples;
    cv::Mat responses;
    auto folder = std::string(forward_).append("/train");

    // iterate forward data

    auto files = Utils::getFiles(folder);

    if (files.empty()) {
      std::cout << "No file found in " << folder << std::endl;
      return;
    }

    std::cout << "Collecting train data in " << folder << std::endl;
    for (auto f : files) {
      auto image = cv::imread(f);

      auto features = easypr::histeq(image);
      features = features.reshape(1, 1);

      samples.push_back(features);
      responses.push_back(Label::kForward);  // Note here
    }

    // iterate inverse data

    files.clear();
    folder = std::string(inverse_).append("/train");
    files = Utils::getFiles(folder);

    if (files.empty()) {
      std::cout << "No file found in " << folder << std::endl;
      return;
    }

    std::cout << "Collecting train data in " << folder << std::endl;
    for (auto f : files) {
      auto image = cv::imread(f);

      auto features = easypr::histeq(image);
      features = features.reshape(1, 1);

      samples.push_back(features);
      responses.push_back(Label::kInverse);  // Note here
    }

    cv::Mat samples_row;
    cv::Mat responses_row;

    samples.convertTo(samples_row, CV_32FC1);
    responses.convertTo(responses_row, CV_32FC1);

    this->trainingData_ = cv::ml::TrainData::create(samples_row, ml::SampleTypes::ROW_SAMPLE, responses_row);
  }

  void SvmTrain::getTest() {
    std::cout << "Tesing preparation..." << std::endl;

    auto folder = std::string(forward_).append("/test");
    auto files = Utils::getFiles(folder);

    size_t size = files.size();
    if (0 == size) {
      std::cout << "No file found in " << folder << std::endl;
      return;
    }
    std::cout << "Prepare testing data in " << folder << std::endl;
    for (auto f : files) {
      auto img = cv::imread(f);
      test_imgaes_.push_back(img);
      test_labels_.push_back(kForward);
    }

    folder.clear();
    folder = std::string(inverse_).append("/test");
    files = Utils::getFiles(folder);

    size = files.size();
    if (0 == size) {
      std::cout << "No file found in " << folder << std::endl;
      return;
    }
    std::cout << "Prepare testing data in " << folder << std::endl;
    for (auto f : files) {
      auto img = cv::imread(f);
      test_imgaes_.push_back(img);
      test_labels_.push_back(kInverse);
    }
  }

}  // namespace easypr
