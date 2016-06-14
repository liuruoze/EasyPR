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
  }

  CharsIdentify::CharsIdentify() {
    ann_ = ml::ANN_MLP::load<ml::ANN_MLP>(kDefaultAnnPath);
    kv_ = std::shared_ptr<Kv>(new Kv);
    kv_->load("etc/province_mapping");
  }

  int CharsIdentify::classify(cv::Mat f, float& maxVal, bool isChinses){
    int result = -1;

    cv::Mat output(1, kCharsTotalNumber, CV_32FC1);
    ann_->predict(f, output);

    maxVal = -2.f;
    if (!isChinses)
    {
      result = 0;
      for (int j = 0; j < kCharactersNumber; j++)
      {
        float val = output.at<float>(j);
        // std::cout << "j:" << j << "val:" << val << std::endl;
        if (val > maxVal)
        {
          maxVal = val;
          result = j;
        }
      }
    }
    else
    {
      result = kCharactersNumber;
      for (int j = kCharactersNumber; j < kCharsTotalNumber; j++)
      {
        float val = output.at<float>(j);
        //std::cout << "j:" << j << "val:" << val << std::endl;
        if (val > maxVal)
        {
          maxVal = val;
          result = j;
        }
      }
    }
    //std::cout << "maxVal:" << maxVal << std::endl;
    return result;
  }

  bool CharsIdentify::isCharacter(cv::Mat input, std::string& label, float& maxVal, bool isChinese) {
    cv::Mat feature = features(input, kPredictSize);
    auto index = static_cast<int>(classify(feature, maxVal, isChinese));

    if (isChinese)
      std::cout << "maxVal:" << maxVal << std::endl;

    if (maxVal >= 0.9) {
      if (index < kCharactersNumber) {
        label = std::make_pair(kChars[index], kChars[index]).second;
      }
      else {
        const char* key = kChars[index];
        std::string s = key;
        std::string province = kv_->get(s);
        label = std::make_pair(s, province).second;
      }
      return true;
    }
    else
      return false;
  }


  std::pair<std::string, std::string> CharsIdentify::identify(cv::Mat input, bool isChinese) {
    cv::Mat feature = features(input, kPredictSize);
    float maxVal = -2;
    auto index = static_cast<int>(classify(feature, maxVal, isChinese));
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
}
