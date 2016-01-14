#ifndef EASYPR_CONFIG_H_
#define EASYPR_CONFIG_H_

namespace easypr {

static const char* kDefaultSvmPath = "resources/model/svm.xml";
static const char* kDefaultAnnPath = "resources/model/ann.xml";

typedef enum {
  kForward = 1, // correspond to "has plate"
  kInverse = 0  // correspond to "no plate"
} SvmLabel;

static const float kSvmPercentage   = 0.7f;
static const int   kPredictSize     = 10;
static const int   kNeurons         = 40;

static const char *kChars[] = {
  "0", "1", "2",
  "3", "4", "5",
  "6", "7", "8",
  "9",
  /*  10  */
  "A", "B", "C",
  "D", "E", "F",
  "G", "H", /* {"I", "I"} */
  "J", "K", "L",
  "M", "N", /* {"O", "O"} */
  "P", "Q", "R",
  "S", "T", "U",
  "V", "W", "X",
  "Y", "Z",
  /*  24  */
  "zh_cuan" , "zh_e"    , "zh_gan"  ,
  "zh_gan1" , "zh_gui"  , "zh_gui1" ,
  "zh_hei"  , "zh_hu"   , "zh_ji"   ,
  "zh_jin"  , "zh_jing" , "zh_jl"   ,
  "zh_liao" , "zh_lu"   , "zh_meng" ,
  "zh_min"  , "zh_ning" , "zh_qing" ,
  "zh_qiong", "zh_shan" , "zh_su"   ,
  "zh_sx"   , "zh_wan"  , "zh_xiang",
  "zh_xin"  , "zh_yu"   , "zh_yu1"  ,
  "zh_yue"  , "zh_yun"  , "zh_zang" ,
  "zh_zhe"
  /*  31  */
};

static const int kCharsTotalNumber = 65;

static bool kDebug = false;

}

#endif // EASYPR_CONFIG_H_