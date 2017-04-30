#ifndef EASYPR_CORE_CHARSIDENTIFY_H_
#define EASYPR_CORE_CHARSIDENTIFY_H_

#include <memory>
#include "opencv2/opencv.hpp"

#include "easypr/util/kv.h"
#include "easypr/core/character.hpp"
#include "easypr/core/feature.h"

namespace easypr {

class CharsIdentify {
public:
  static CharsIdentify* instance();

  int classify(cv::Mat f, float& maxVal, bool isChinses = false, bool isAlphabet = false);
  void classify(cv::Mat featureRows, std::vector<int>& out_maxIndexs,
                std::vector<float>& out_maxVals, std::vector<bool> isChineseVec);
  void classify(std::vector<CCharacter>& charVec);

  void classifyChinese(std::vector<CCharacter>& charVec);
  void classifyChineseGray(std::vector<CCharacter>& charVec);

  std::pair<std::string, std::string> identify(cv::Mat input, bool isChinese = false, bool isAlphabet = false);
  int identify(std::vector<cv::Mat> inputs, std::vector<std::pair<std::string, std::string>>& outputs,
               std::vector<bool> isChineseVec);

  std::pair<std::string, std::string> identifyChinese(cv::Mat input, float& result, bool& isChinese);
  std::pair<std::string, std::string> identifyChineseGray(cv::Mat input, float& result, bool& isChinese);

  bool isCharacter(cv::Mat input, std::string& label, float& maxVal, bool isChinese = false);

  void LoadModel(std::string path);
  void LoadChineseModel(std::string path);
  void LoadGrayChANN(std::string path);
  void LoadChineseMapping(std::string path);

private:
  CharsIdentify();
  annCallback extractFeature;
  static CharsIdentify* instance_;

  // binary character classifer
  cv::Ptr<cv::ml::ANN_MLP> ann_;

  // binary character classifer, only for chinese
  cv::Ptr<cv::ml::ANN_MLP> annChinese_;

  // gray classifer, only for chinese
  cv::Ptr<cv::ml::ANN_MLP> annGray_;

  // used for chinese mapping
  std::shared_ptr<Kv> kv_;
};
}

#endif  //  EASYPR_CORE_CHARSIDENTIFY_H_