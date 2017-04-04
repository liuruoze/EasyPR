#include <numeric>
#include <ctime>

#include "easypr/train/annCh_train.h"
#include "easypr/config.h"
#include "easypr/core/chars_identify.h"
#include "easypr/core/feature.h"
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"
#include "easypr/train/create_data.h"

namespace easypr {

  AnnChTrain::AnnChTrain(const char* chars_folder, const char* xml)
    : chars_folder_(chars_folder), ann_xml_(xml) {
  ann_ = cv::ml::ANN_MLP::create();
  type = 1;
  kv_ = std::shared_ptr<Kv>(new Kv);
  kv_->load("etc/province_mapping");
  extractFeature = getGrayCharFeatures;
}

  void AnnChTrain::train() {
  int classNumber = 0;
  int input_number = 0;
  int hidden_number = 0;
  int output_number = 0;

  classNumber = kChineseNumber;
  input_number = kGrayCharHeight * kGrayCharWidth;
  hidden_number = 64;
  output_number = classNumber;
  cv::Mat layers;
  layers.create(1, 3, CV_32SC1);
  layers.at<int>(0) = input_number;
  layers.at<int>(1) = hidden_number;
  layers.at<int>(2) = output_number;
  
  ann_->setLayerSizes(layers);
  ann_->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
  ann_->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::RPROP);
  ann_->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));
  ann_->setBackpropWeightScale(0.05);
  ann_->setBackpropMomentumScale(0.9);

  // using raw data or raw + synthic data.
  trainVal(350);
}

std::pair<std::string, std::string> AnnChTrain::identifyChinese(cv::Mat input) {
  Mat feature;
  extractFeature(input, feature);
  float maxVal = -2;
  int result = -1;

  cv::Mat output(1, kChineseNumber, CV_32FC1);
  ann_->predict(feature, output);

  for (int j = 0; j < kChineseNumber; j++) {
    float val = output.at<float>(j);
    // std::cout << "j:" << j << "val:" << val << std::endl;
    if (val > maxVal) {
      maxVal = val;
      result = j;
    }
  }

  auto index = result + kCharsTotalNumber - kChineseNumber;
  const char* key = kChars[index];
  std::string s = key;
  std::string province = kv_->get(s);

  return std::make_pair(s, province);
}

  void AnnChTrain::test() {
    //TODO
}

cv::Mat AnnChTrain::generateGraySyntheticImage(const Mat& image) {
  int rand_type = rand();
  Mat result = image.clone();

  if (rand_type % 2 == 0) {
    int ran_x = rand() % 5 - 2;
    int ran_y = rand() % 5 - 2;

    result = translateImg(result, ran_x, ran_y);
  }
  else if (rand_type % 2 != 0) {
    float angle = float(rand() % 15 - 7);

    result = rotateImg(result, angle);
  }
  
  return result;
}

void AnnChTrain::trainVal(size_t number_for_count) {
  assert(chars_folder_);
  cv::Mat train_samples;
  std::vector<cv::Mat>  val_images;
  std::vector<int> train_label, val_labels;
  float percentage = 0.7f;
  int classNumber = kChineseNumber;

  for (int i = 0; i < classNumber; ++i) { 
    auto char_key = kChars[i + kCharsTotalNumber - classNumber];
    char sub_folder[512] = { 0 };
    sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
    fprintf(stdout, ">> Testing characters %s in %s \n", char_key, sub_folder);
    auto chars_files = utils::getFiles(sub_folder);
    size_t char_size = chars_files.size();
    fprintf(stdout, ">> Characters count: %d \n", char_size);

    std::vector<cv::Mat> matVec;
    matVec.reserve(number_for_count);
    for (auto file : chars_files) {
      auto img = cv::imread(file, IMREAD_GRAYSCALE);  // a grayscale image
      matVec.push_back(img);
    }
    // genrate the synthetic images
    /*for (int t = 0; t < (int)number_for_count - (int)char_size; t++) {
      int rand_range = char_size + t;
      int ran_num = rand() % rand_range;
      auto img = matVec.at(ran_num);
      auto simg = generateGraySyntheticImage(img);
      matVec.push_back(simg);
    }*/
    fprintf(stdout, ">> Characters count: %d \n", matVec.size());

    // random sort the mat;
    srand(unsigned(time(NULL)));
    //random_shuffle(matVec.begin(), matVec.end());

    size_t split_index = size_t(matVec.size() * percentage);
    for (size_t i = 0; i < matVec.size(); ++i) {
      Mat img = matVec.at(i);
      Mat feature;
      extractFeature(img, feature);
      if (i <= split_index) {
        train_samples.push_back(feature);
        train_label.push_back(i);
      } 
      else {
        val_images.push_back(img);
        val_labels.push_back(i);
      }
    }
  } 
  // generate train data
  train_samples.convertTo(train_samples, CV_32F);
  cv::Mat train_classes = cv::Mat::zeros((int)train_label.size(), classNumber, CV_32F);
  for (int i = 0; i < train_classes.rows; ++i)
    train_classes.at<float>(i, train_label[i]) = 1.f;
  auto train_data = cv::ml::TrainData::create(train_samples, cv::ml::SampleTypes::ROW_SAMPLE, train_classes);

  // train the data, calculate the cost time
  std::cout << "Training ANN chinese model, please wait..." << std::endl;
  long start = utils::getTimestamp();
  ann_->train(train_data);
  long end = utils::getTimestamp();
  ann_->save(ann_xml_);
  std::cout << "Your ANN Model was saved to " << ann_xml_ << std::endl;
  std::cout << "Training done. Time elapse: " << (end - start) / (1000 * 60) << "minute" << std::endl;

  // test the accuracy_rate
  int corrects_all = 0, sum_all = val_images.size();
  for (size_t i = 0; i < val_images.size(); ++i) {
    cv::Mat img = val_images.at(i);
    int label = val_labels.at(i);
    auto char_key = kChars[i + kCharsTotalNumber - classNumber];
    std::pair<std::string, std::string> ch = identifyChinese(img);
    if (ch.first == char_key) 
      corrects_all++;
  }
  float accuracy_rate = (float)corrects_all / (float)sum_all;
  std::cout << "Accuracy_rate: " << accuracy_rate << std::endl;
}

cv::Ptr<cv::ml::TrainData> AnnChTrain::tdata() {
  assert(chars_folder_);

  cv::Mat samples;
  std::vector<int> labels;

  std::cout << "Collecting chars in " << chars_folder_ << std::endl;

  int classNumber = 0;
  if (type == 0) classNumber = kCharsTotalNumber;
  if (type == 1) classNumber = kChineseNumber;

  for (int i = 0; i < classNumber; ++i) {
    auto char_key = kChars[i + kCharsTotalNumber - classNumber];
    char sub_folder[512] = {0};

    sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
    std::cout << "  >> Featuring characters " << char_key << " in "
              << sub_folder << std::endl;

    auto chars_files = utils::getFiles(sub_folder);
    for (auto file : chars_files) {
      auto img = cv::imread(file, 0);  // a grayscale image
      auto fps = charFeatures2(img, kPredictSize);

      samples.push_back(fps);
      labels.push_back(i);
    }
  }

  cv::Mat samples_;
  samples.convertTo(samples_, CV_32F);
  cv::Mat train_classes =
    cv::Mat::zeros((int)labels.size(), classNumber, CV_32F);

  for (int i = 0; i < train_classes.rows; ++i) {
    train_classes.at<float>(i, labels[i]) = 1.f;
  }

  return cv::ml::TrainData::create(samples_, cv::ml::SampleTypes::ROW_SAMPLE,
                                   train_classes);
}
}
