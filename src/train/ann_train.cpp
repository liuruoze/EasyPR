#include "easypr/train/ann_train.h"
#include "easypr/config.h"
#include "easypr/core/chars_identify.h"
#include "easypr/core/feature.h"
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"
#include <numeric>
#include <ctime>

namespace easypr {

AnnTrain::AnnTrain(const char* chars_folder, const char* xml)
    : chars_folder_(chars_folder), ann_xml_(xml) {
  ann_ = cv::ml::ANN_MLP::create();
  type = 1;
  kv_ = std::shared_ptr<Kv>(new Kv);
  kv_->load("etc/province_mapping");
}

void AnnTrain::train() {
  int classNumber = 0;

  cv::Mat layers;

  int input_number = 0;
  int hidden_number = 0;
  int output_number = 0;

  if (type == 0) {
    classNumber = kCharsTotalNumber;

    input_number = kAnnInput;
    hidden_number = kNeurons;
    output_number = classNumber;
  }
  else if (type == 1) {
    classNumber = kChineseNumber;

    input_number = kAnnInput;
    hidden_number = kNeurons;
    output_number = classNumber;
  }

  int N = input_number;
  int m = output_number;
  int first_hidden_neurons = int(std::sqrt((m + 2) * N) + 2 * std::sqrt(N / (m + 2)));
  int second_hidden_neurons = int(m * std::sqrt(N / (m + 2)));

  bool useTLFN = false;
  if (!useTLFN) {
    layers.create(1, 3, CV_32SC1);
    layers.at<int>(0) = input_number;
    layers.at<int>(1) = hidden_number;
    layers.at<int>(2) = output_number;
  }
  else {
    fprintf(stdout, ">> Use two-layers neural networks,\n");
    fprintf(stdout, ">> First_hidden_neurons: %d \n", first_hidden_neurons);
    fprintf(stdout, ">> Second_hidden_neurons: %d \n", second_hidden_neurons);

    layers.create(1, 4, CV_32SC1);
    layers.at<int>(0) = input_number;
    layers.at<int>(1) = first_hidden_neurons;
    layers.at<int>(2) = second_hidden_neurons;
    layers.at<int>(3) = output_number;
  }

  ann_->setLayerSizes(layers);
  ann_->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
  ann_->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
  ann_->setTermCriteria(cvTermCriteria(CV_TERMCRIT_ITER, 30000, 0.0001));
  ann_->setBackpropWeightScale(0.1);
  ann_->setBackpropMomentumScale(0.1);

  //using raw data or raw + synthic data.
  //auto traindata = tdata();
  auto traindata = sdata(350);

  std::cout << "Training ANN model, please wait..." << std::endl;
  long start = utils::getTimestamp();
  ann_->train(traindata);
  long end = utils::getTimestamp();
  ann_->save(ann_xml_);
 
  test();
  std::cout << "Your ANN Model was saved to " << ann_xml_ << std::endl;
  std::cout << "Training done. Time elapse: " << (end - start) / (1000 * 60) << "minute"
    << std::endl;
}

std::pair<std::string, std::string> AnnTrain::identifyChinese(cv::Mat input) {
  cv::Mat feature = charFeatures2(input, kPredictSize);
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


std::pair<std::string, std::string> AnnTrain::identify(cv::Mat input) {
  cv::Mat feature = charFeatures2(input, kPredictSize);
  float maxVal = -2;
  int result = -1;

  cv::Mat output(1, kCharsTotalNumber, CV_32FC1);
  ann_->predict(feature, output);

  for (int j = 0; j < kCharsTotalNumber; j++) {
    float val = output.at<float>(j);
    // std::cout << "j:" << j << "val:" << val << std::endl;
    if (val > maxVal) {
      maxVal = val;
      result = j;
    }
  }

  auto index = result;
  if (index < kCharactersNumber) {
    return std::make_pair(kChars[index], kChars[index]);
  }
  else {
    const char* key = kChars[index];
    std::string s = key;
    std::string province = kv_->get(s);
    return std::make_pair(s, province);
  }
}

void AnnTrain::test() {
  assert(chars_folder_);

  int classNumber = 0;
  if (type == 0) classNumber = kCharsTotalNumber;
  if (type == 1) classNumber = kChineseNumber;

  int corrects_all = 0, sum_all = 0;
  std::vector<float> rate_list;
  for (int i = 0; i < classNumber; ++i) {
    auto char_key = kChars[i + kCharsTotalNumber - classNumber];
    char sub_folder[512] = { 0 };

    sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
    fprintf(stdout, ">> Testing characters %s in %s \n", char_key, sub_folder);

    auto chars_files = utils::getFiles(sub_folder);
    int corrects = 0, sum = 0;
    std::vector<std::pair<std::string, std::string>> error_files;

    for (auto file : chars_files) {
      auto img = cv::imread(file, 0);  // a grayscale image
      std::pair<std::string, std::string> ch;

      if (type == 0) ch = identify(img);
      if (type == 1) ch = identifyChinese(img);

      if (ch.first == char_key) {
        ++corrects;
        ++corrects_all;
      } else {
        error_files.push_back(
            std::make_pair(utils::getFileName(file), ch.second));
      }
      ++sum;
      ++sum_all;
    }
    float rate = (float)corrects / (sum == 0 ? 1 : sum);
    fprintf(stdout, ">>   [sum: %d, correct: %d, rate: %.2f]\n", sum, corrects, rate);
    rate_list.push_back(rate);

    std::string error_string;
    auto end = error_files.end();
    if (error_files.size() >= 10) {
      end -= static_cast<size_t>(error_files.size() * (1 - 0.1));
    }
    for (auto k = error_files.begin(); k != end; ++k) {
      auto kv = *k;
      error_string.append("       ").append(kv.first).append(": ").append(
          kv.second);
      if (k != end - 1) {
        error_string.append(",\n");
      } else {
        error_string.append("\n       ...");
      }
    }
    fprintf(stdout, ">>   [\n%s\n     ]\n", error_string.c_str());
  }
  fprintf(stdout, ">>   [sum_all: %d, correct_all: %d, rate: %.4f]\n", sum_all, corrects_all,
    (float)corrects_all / (sum_all == 0 ? 1 : sum_all));

  double rate_sum = std::accumulate(rate_list.begin(), rate_list.end(), 0.0);
  double rate_mean = rate_sum / (rate_list.size() == 0 ? 1 : rate_list.size());

  fprintf(stdout, ">>   [classNumber: %d, avg_rate: %.4f]\n", classNumber, rate_mean);
}

cv::Mat getSyntheticImage(const Mat& image) {
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

cv::Ptr<cv::ml::TrainData> AnnTrain::sdata(size_t number_for_count) {
  assert(chars_folder_);

  cv::Mat samples;
  std::vector<int> labels;

  int classNumber = 0;
  if (type == 0) classNumber = kCharsTotalNumber;
  if (type == 1) classNumber = kChineseNumber;
  
  srand((unsigned)time(0));
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
      auto img = cv::imread(file, 0);  // a grayscale image
      matVec.push_back(img);
    }

    for (int t = 0; t < (int)number_for_count - (int)char_size; t++) {
      int rand_range = char_size + t;
      int ran_num = rand() % rand_range;
      auto img = matVec.at(ran_num);
      auto simg = getSyntheticImage(img);
      matVec.push_back(simg);
      if (1) {
        std::stringstream ss(std::stringstream::in | std::stringstream::out);
        ss << sub_folder << "/" << i << "_" << t << "_" << ran_num << ".jpg";
        imwrite(ss.str(), simg);
      }
    }

    fprintf(stdout, ">> Characters count: %d \n", matVec.size());

    for (auto img : matVec) {
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

cv::Ptr<cv::ml::TrainData> AnnTrain::tdata() {
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
