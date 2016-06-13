#include "easypr/train/ann_train.h"
#include "easypr/config.h"
#include "easypr/core/chars_identify.h"
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"

namespace easypr {

AnnTrain::AnnTrain(const char* chars_folder, const char* xml)
    : chars_folder_(chars_folder), ann_xml_(xml) {
  ann_ = cv::ml::ANN_MLP::create();
}

void AnnTrain::train() {
  cv::Mat layers(1, 3, CV_32SC1);
  layers.at<int>(0) = 120;                // the input layer
  layers.at<int>(1) = kNeurons;           // the neurons
  layers.at<int>(2) = kCharsTotalNumber;  // the output layer

  ann_->setLayerSizes(layers);
  ann_->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 1, 1);
  ann_->setTrainMethod(cv::ml::ANN_MLP::TrainingMethods::BACKPROP);
  ann_->setBackpropWeightScale(0.1);
  ann_->setBackpropMomentumScale(0.1);

  auto traindata = tdata();
  std::cout << "Training ANN model, please wait..." << std::endl;
  long start = utils::getTimestamp();
  ann_->train(traindata);
  long end = utils::getTimestamp();
  std::cout << "Training done. Time elapse: " << (end - start) << "ms"
            << std::endl;

  ann_->save(ann_xml_);
  std::cout << "Your ANN Model was saved to " << ann_xml_ << std::endl;
}

void AnnTrain::test() {
  assert(chars_folder_);

  for (int i = 0; i < kCharsTotalNumber; ++i) {
    auto char_key = kChars[i];
    char sub_folder[512] = {0};

    sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
    fprintf(stdout, ">> Testing characters %s in %s \n", char_key, sub_folder);

    auto chars_files = utils::getFiles(sub_folder);
    int corrects = 0, sum = 0;
    std::vector<std::pair<std::string, std::string>> error_files;

    for (auto file : chars_files) {
      auto img = cv::imread(file, 0);  // a grayscale image
      std::pair<std::string, std::string> ch =
          CharsIdentify::instance()->identify(img);
      if (ch.first == char_key) {
        ++corrects;
      } else {
        error_files.push_back(
            std::make_pair(utils::getFileName(file), ch.second));
      }
      ++sum;
    }
    fprintf(stdout, ">>   [sum: %d, correct: %d, rate: %.2f]\n", sum, corrects,
            (float)corrects / (sum == 0 ? 1 : sum));
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
}

cv::Ptr<cv::ml::TrainData> AnnTrain::tdata() {
  assert(chars_folder_);

  cv::Mat samples;
  std::vector<int> labels;

  std::cout << "Collecting chars in " << chars_folder_ << std::endl;

  for (int i = 0; i < kCharsTotalNumber; ++i) {
    auto char_key = kChars[i];
    char sub_folder[512] = {0};

    sprintf(sub_folder, "%s/%s", chars_folder_, char_key);
    std::cout << "  >> Featuring characters " << char_key << " in "
              << sub_folder << std::endl;

    auto chars_files = utils::getFiles(sub_folder);
    for (auto file : chars_files) {
      auto img = cv::imread(file, 0);  // a grayscale image
      auto fps = features(img, kPredictSize);

      samples.push_back(fps);
      labels.push_back(i);
    }
  }

  cv::Mat samples_;
  samples.convertTo(samples_, CV_32F);
  cv::Mat train_classes =
      cv::Mat::zeros((int)labels.size(), kCharsTotalNumber, CV_32F);

  for (int i = 0; i < train_classes.rows; ++i) {
    train_classes.at<float>(i, labels[i]) = 1.f;
  }

  return cv::ml::TrainData::create(samples_, cv::ml::SampleTypes::ROW_SAMPLE,
                                   train_classes);
}
}
