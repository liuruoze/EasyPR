#ifndef EASYPR_TRAIN_TRAIN_H_
#define EASYPR_TRAIN_TRAIN_H_

namespace easypr{

  static const char *kDefaultSvmPath = "resources/model/svm.xml";
  static const char *kDefaultAnnPath = "resources/model/ann.xml";

  static const int kPredictSize = 10;

  static const char * kChinese[] = {
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

  static const char kCharacters[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', /* Ã»ÓÐI */
    'J', 'K', 'L', 'M', 'N', /* Ã»ÓÐO */ 'P', 'Q',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
  };

}



#endif // EASYPR_TRAIN_TRAIN_H_