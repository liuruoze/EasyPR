#ifndef EASYPR_CONFIG_H_
#define EASYPR_CONFIG_H_

namespace easypr {

  enum Color { BLUE, YELLOW, WHITE, UNKNOWN };

  enum LocateType { SOBEL, COLOR, CMSER, OTHER };

  enum CharSearchDirection { LEFT, RIGHT };

  enum
  {
    PR_DETECT_SOBEL = 0x01,  /**Sobel detect type, using twice Sobel  */
    PR_DETECT_COLOR = 0x02,  /**Color detect type   */
    PR_DETECT_CMSER = 0x04,  /**Character detect type, using mser  */
  };

static const char* kDefaultSvmPath = "resources/model/svm.xml";
static const char* kLBPSvmPath = "resources/model/svm_lbp_final.xml";
static const char* kDefaultAnnPath = "resources/model/ann.xml";
static const char* kChineseAnnPath = "resources/model/ann_chinese.xml";

typedef enum {
  kForward = 1, // correspond to "has plate"
  kInverse = 0  // correspond to "no plate"
} SvmLabel;

static const int   kPlateResizeWidth = 136;
static const int   kPlateResizeHeight = 36;

static const int   kShowWindowWidth = 800;
static const int   kShowWindowHeight = 600;

static const float kSvmPercentage   = 0.7f;

static const int   kCharacterInput  = 120;
static const int   kChineseInput    = 440;
static const int   kAnnInput = kCharacterInput;

static const int   kCharacterSize = 10;
static const int   kChineseSize = 20;
static const int   kPredictSize = kCharacterSize;

static const int   kNeurons       = 40;

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

static const int kCharactersNumber = 34;
static const int kChineseNumber = 31;
static const int kCharsTotalNumber = 65;

static bool kDebug = false;

}

#endif // EASYPR_CONFIG_H_