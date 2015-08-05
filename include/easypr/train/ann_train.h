#ifndef EASYPR_TRAIN_ANNTRAIN_H_
#define EASYPR_TRAIN_ANNTRAIN_H_

#include <opencv2/opencv.hpp>
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"

namespace easypr{

  const char* kChinese[] = {
    "zh_cuan" /* ´¨ */, "zh_e" /* ¶õ */, "zh_gan" /* ¸Ó*/,
    "zh_gan1" /*¸Ê*/, "zh_gui" /* ¹ó */, "zh_gui1" /* ¹ð */,
    "zh_hei" /* ºÚ */, "zh_hu" /* »¦ */, "zh_ji" /* ¼½ */,
    "zh_jin" /* ½ò */, "zh_jing" /* ¾© */, "zh_jl" /* ¼ª */,
    "zh_liao" /* ÁÉ */, "zh_lu" /* Â³ */, "zh_meng" /* ÃÉ */,
    "zh_min" /* Ãö */, "zh_ning" /* Äþ */, "zh_qing" /* Çà */,
    "zh_qiong" /* Çí */, "zh_shan" /* ÉÂ */, "zh_su" /* ËÕ */,
    "zh_sx" /* ½ú */, "zh_wan" /* Íî */, "zh_xiang" /* Ïæ */,
    "zh_xin" /* ÐÂ */, "zh_yu" /* Ô¥ */, "zh_yu1" /* Óå */,
    "zh_yue" /* ÔÁ */, "zh_yun" /* ÔÆ */, "zh_zang" /* ²Ø */,
    "zh_zhe" /* Õã */
  };

  const char kCharacters[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', /* Ã»ÓÐI */
    'J', 'K', 'L', 'M', 'N', /* Ã»ÓÐO */ 'P', 'Q',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
  };

  const int kPredictSize = 10;

  class AnnTrain{
  public:
    explicit AnnTrain(const char* chars_folder, const char* zhchars_folder, const char* xml);

    void train(const int & neurons = 40);

  private:
    void getTrainData();

    cv::Ptr<cv::ml::ANN_MLP> ann_;
    cv::Ptr<cv::ml::TrainData> train_data_;

    const char* ann_xml_;
    const char* chars_folder_;
    const char* zhchars_folder_;
  };

}

#endif // EASYPR_TRAIN_ANNTRAIN_H_