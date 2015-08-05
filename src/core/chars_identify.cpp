#include "easypr/core/chars_identify.h"
#include "easypr/core/core_func.h"
#include "easypr/train/ann_train.h"

namespace easypr {

  std::map<string, string> zh_map = {
    { "zh_cuan", "川" },
    { "zh_e", "鄂" },
    { "zh_gan", "赣" },
    { "zh_gan1", "甘" },
    { "zh_gui", "贵" },
    { "zh_gui1", "桂" },
    { "zh_hei", "黑" },
    { "zh_hu", "沪" },
    { "zh_ji", "冀" },
    { "zh_jin", "津" },
    { "zh_jing", "京" },
    { "zh_jl", "吉" },
    { "zh_liao", "辽" },
    { "zh_lu", "鲁" },
    { "zh_meng", "蒙" },
    { "zh_min", "闽" },
    { "zh_ning", "宁" },
    { "zh_qing", "青" },
    { "zh_qiong", "琼" },
    { "zh_shan", "陕" },
    { "zh_su", "苏" },
    { "zh_sx", "晋" },
    { "zh_wan", "皖" },
    { "zh_xiang", "湘" },
    { "zh_xin", "新" },
    { "zh_yu", "豫" },
    { "zh_yu1", "渝" },
    { "zh_yue", "粤" },
    { "zh_yun", "云" },
    { "zh_zang", "藏" },
    { "zh_zhe", "浙" }
  };

  CharsIdentify* CharsIdentify::instance_ = nullptr;

  CharsIdentify* CharsIdentify::instance(){
    if (!instance_){
      instance_ = new CharsIdentify;
    }
    return instance_;
  }

  CharsIdentify::CharsIdentify() {
    ann_ = ml::ANN_MLP::load<ml::ANN_MLP>("resources/model/ann.xml");
  }

  string CharsIdentify::identify(Mat input, bool isChinese, bool isSpeci) {
    Mat f = features(input, kPredictSize);

    string result;
    int index = classify(f, isChinese, isSpeci);

    if (!isChinese) {
      result = result + kCharacters[index];
    }
    else {
      string s = kChinese[index - sizeof(kCharacters)];
      string province = zh_map[s];
      result = province + result;
    }

    return result;
  }

  int CharsIdentify::classify(Mat f, bool isChinses, bool isSpeci) {
    int result = -1;
    Mat output(1, sizeof(kCharacters) + sizeof(kChinese), CV_32FC1);
    ann_->predict(f, output);  //使用ann对字符做判断

    if (!isChinses)  // 对数字和英文字母的判断
    {
      if (isSpeci) {
        // A-Z
        result = 0;
        float maxVal = -2;
        for (int j = 10; j < sizeof(kCharacters); j++) {
          float val = output.at<float>(j);
          // cout << "j:" << j << "val:"<< val << endl;
          if (val > maxVal) {
            maxVal = val;
            result = j;
          }
        }
      }
      else {
        // 0-9
        result = 0;
        float maxVal = -2;
        for (int j = 0; j < sizeof(kCharacters); j++) {
          float val = output.at<float>(j);
          // cout << "j:" << j << "val:"<< val << endl;
          if (val >
            maxVal)  //求得中文字符权重最大的那个，也就是通过ann认为最可能的字符
          {
            maxVal = val;
            result = j;
          }
        }
      }
    }
    else  // 对中文字符的判断
    {
      result = sizeof(kCharacters);
      float maxVal = -2;
      for (int j = sizeof(kCharacters); j < sizeof(kCharacters) + sizeof(sizeof(kChinese)); j++) {
        float val = output.at<float>(j);
        // cout << "j:" << j << "val:"<< val << endl;
        if (val > maxVal) {
          maxVal = val;
          result = j;
        }
      }
    }
    return result;
  }

  //int CCharsIdentify::classify(Mat f) {
  //  int result = -1;
  //  Mat output(1, numAll, CV_32FC1);
  //  ann.predict(f, output);  //使用ann对字符做判断

  //  float maxVal = -2;
  //  for (int j = 0; j < numCharacter; j++) {
  //    float val = output.at<float>(j);
  //    // cout << "j:" << j << "val:"<< val << endl;
  //    if (val > maxVal)
  //      //求得中文字符权重最大的那个，也就是通过ann认为最可能的字符
  //    {
  //      maxVal = val;
  //      result = j;
  //    }
  //  }

  //  for (int j = numCharacter; j < numAll; j++) {
  //    float val = output.at<float>(j);
  //    // cout << "j:" << j << "val:"<< val << endl;
  //    if (val > maxVal) {
  //      maxVal = val;
  //      result = j;
  //    }
  //  }

  //  return result;
  //}

  //string CCharsIdentify::charsIdentify(Mat input) {
  //  Mat f = features(input, m_predictSize);
  //  string result = "";
  //  int index = classify(f);  //使用ann来判别那个字符

  //  if (index >= numCharacter) {
  //    string s = strChinese[index - numCharacter];
  //    string province = m_map[s];
  //    return s;
  //  }
  //  else {
  //    char s = strCharacters[index];
  //    char szBuf[216];
  //    sprintf(szBuf, "%c", s);
  //    return szBuf;
  //  }
  //}

}
