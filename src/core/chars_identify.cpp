#include "easypr/core/chars_identify.h"
#include "easypr/config.h"
#include "easypr/core/core_func.h"

namespace easypr {

CharsIdentify* CharsIdentify::instance_ = nullptr;
CharsIdentify* CharsIdentify::instance() {
  if (!instance_) {
    instance_ = new CharsIdentify;
  }
  return instance_;
CharsIdentify::CharsIdentify() {
  ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(kDefaultAnnPath);
  kv_ = std::shared_ptr<Kv>(new Kv);
  kv_->load("etc/province_mapping");
int CharsIdentify::classify(cv::Mat f, bool isChinses){
  cv::Mat output(1, kCharsTotalNumber, CV_32FC1);
  ann_->predict(f, output);
  if (!isChinses)
  {
    result = 0;
    float maxVal = -2;
    for (int j = 0; j < kCharactersNumber; j++)
    {
      float val = output.at<float>(j);
      //cout << "j:" << j << "val:"<< val << endl;
      if (val > maxVal)
      {
        maxVal = val;
        result = j;
      }
    }
  }
  else
    result = kCharactersNumber;
    float maxVal = -2;
    for (int j = kCharactersNumber; j < kCharsTotalNumber; j++)
    {
      float val = output.at<float>(j);
      //cout << "j:" << j << "val:"<< val << endl;
      if (val > maxVal)
      {
        maxVal = val;
        result = j;
      }
    }
  return instance_;
}
CharsIdentify::CharsIdentify() {
  ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(kDefaultAnnPath);
  kv_ = std::shared_ptr<Kv>(new Kv);
  kv_->load("etc/province_mapping");
}

    return std::make_pair(kChars[index], kChars[index]);
std::pair<std::string, std::string> CharsIdentify::identify(cv::Mat input, bool isChinese) {
  cv::Mat feature = features(input, kPredictSize);
  auto index = static_cast<int>(classify(feature, isChinese));
  if (index < kCharactersNumber) {
    return std::make_pair(kChars[index], kChars[index]);
  } else {
    const char* key = kChars[index];
    std::string s = key;
    std::string province = kv_->get(s);
    return std::make_pair(s, province);
  }
}
}
