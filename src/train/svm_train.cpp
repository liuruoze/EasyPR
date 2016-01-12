#include "easypr/svm_train.h"
#include <ctime>
#include "easypr/core_func.h"
#include "easypr/feature.h"
#include "easypr/util.h"

namespace easypr {

  Svm::Svm(const char* forward_data_folder, const char* inverse_data_folder)
    : forward_(forward_data_folder), inverse_(inverse_data_folder) {
    assert(forward_);
    assert(inverse_);
  }

  void Svm::divide(const char* images_folder, float percentage /* = 0.7 */) {
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

  void Svm::get_train() {
    std::cout << "Collecting train data..." << std::endl;

    std::vector<int> labels;
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

      //auto features = easypr::histeq(image);
      //features = features.reshape(1, 1);

      Mat features;
      getHistogramFeatures(image, features);
      features = features.reshape(1, 1);

      this->trainingData_.push_back(features);
      labels.push_back(Label::kForward);  // Note here
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

      /*auto features = easypr::histeq(image);
      features = features.reshape(1, 1);*/

      Mat features;
      getHistogramFeatures(image, features);
      features = features.reshape(1, 1);

      this->trainingData_.push_back(features);
      labels.push_back(Label::kInverse);  // Note here
    }

    //
    cv::Mat out;
    this->trainingData_.convertTo(out, CV_32FC1);
    out.copyTo(this->trainingData_);
    cv::Mat(labels).copyTo(this->classes_);
  }

  void Svm::get_test() {
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

    //

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

  void Svm::train(bool divide /* = true */, float divide_percentage /* = 0.7 */,
    bool train /* = true */,
    const char* out_svm_path /* = NULL */)
  {

    if (out_svm_path == NULL) {
      out_svm_path = "resources/train/svm.xml";
    }

    if (divide) {
      std::cout << "Dividing data to be trained and tested..." << std::endl;
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
        // SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
        // 线型，也就是无核
        SVM_params.kernel_type =
          CvSVM::RBF;  // CvSVM::RBF 径向基函数，也就是高斯核
        SVM_params.degree = 0.1;
        SVM_params.gamma = 0.1;
        SVM_params.coef0 = 0.1;
        SVM_params.C = 1;
        SVM_params.nu = 0.1;
        SVM_params.p = 0.1;
        SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100000, 0.0001);

        std::cout << "Generating svm model file, please wait..." << std::endl;

        try {
          // CvSVM svm(trainingData, classes, cv::Mat(), cv::Mat(), SVM_params);
          /*svm.train_auto(this->trainingData_, this->classes_, cv::Mat(),
          cv::Mat(), SVM_params, 10,
          CvSVM::get_default_grid(CvSVM::C),
          CvSVM::get_default_grid(CvSVM::GAMMA),
          CvSVM::get_default_grid(CvSVM::P),
          CvSVM::get_default_grid(CvSVM::NU),
          CvSVM::get_default_grid(CvSVM::COEF),
          CvSVM::get_default_grid(CvSVM::DEGREE), true);*/
          svm.train(this->trainingData_, this->classes_, cv::Mat(),
            cv::Mat(), SVM_params);
        }
        catch (const cv::Exception& err) {
          std::cout << err.what() << std::endl;
        }

        //utils::mkdir(out_svm_path);
        cv::FileStorage fsTo(out_svm_path, cv::FileStorage::WRITE);
        svm.write(*fsTo, "svm");

        std::cout << "Generate done! The model file is located at "
          << out_svm_path << std::endl;
      }
      else {
        // don't train, use ready-made model file
        try {
          svm.load("resources/train/svm.xml", "svm");
        }
        catch (const cv::Exception& err) {
          std::cout << err.what() << std::endl;
        }
      }
    }  // if train

    // TODO Check whether the model file exists or not.
    //svm.load(out_svm_path, "svm");  // make sure svm model was loaded

    // 30% testing procedure
    this->get_test();

    std::cout << "Testing..." << std::endl;

    double count_all = test_imgaes_.size();
    double ptrue_rtrue = 0;
    double ptrue_rfalse = 0;
    double pfalse_rtrue = 0;
    double pfalse_rfalse = 0;

    size_t label_index = 0;
    for (auto image : test_imgaes_) {
      //调用回调函数决定特征
      //auto features = easypr::histeq(image);

      Mat features;
      getHistogramFeatures(image, features);
      features = features.reshape(1, 1);
      features.convertTo(features, CV_32FC1);

      Label predict = ((int)svm.predict(features)) == 1 ? kForward : kInverse;
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

}  // namespace easypr
