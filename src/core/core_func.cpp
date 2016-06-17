// 这个文件定义了EasyPR里所有plate判断的通用函数
// 所属命名空间为easypr
// 这个部分中的函数轻易不要改动

#include "easypr/core/core_func.h"
#include "easypr/core/character.hpp"
#include "easypr/core/chars_identify.h"

using namespace cv;

namespace easypr {

//! 根据一幅图像与颜色模板获取对应的二值图
//! 输入RGB图像, 颜色模板（蓝色、黄色）
//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）

Mat colorMatch(const Mat &src, Mat &match, const Color r,
               const bool adaptive_minsv) {

  // S和V的最小值由adaptive_minsv这个bool值判断
  // 如果为true，则最小值取决于H值，按比例衰减
  // 如果为false，则不再自适应，使用固定的最小值minabs_sv
  // 默认为false

  const float max_sv = 255;
  const float minref_sv = 64;

  const float minabs_sv = 95;

  // blue的H范围

  const int min_blue = 100;  // 100
  const int max_blue = 140;  // 140

  // yellow的H范围

  const int min_yellow = 15;  // 15
  const int max_yellow = 40;  // 40

  // white的H范围

  const int min_white = 0;   // 15
  const int max_white = 30;  // 40

  Mat src_hsv;

  // 转到HSV空间进行处理，颜色搜索主要使用的是H分量进行蓝色与黄色的匹配工作

  cvtColor(src, src_hsv, CV_BGR2HSV);

  std::vector<cv::Mat> hsvSplit;
  split(src_hsv, hsvSplit);
  equalizeHist(hsvSplit[2], hsvSplit[2]);
  merge(hsvSplit, src_hsv);

  //匹配模板基色,切换以查找想要的基色

  int min_h = 0;
  int max_h = 0;
  switch (r) {
    case BLUE:
      min_h = min_blue;
      max_h = max_blue;
      break;
    case YELLOW:
      min_h = min_yellow;
      max_h = max_yellow;
      break;
    case WHITE:
      min_h = min_white;
      max_h = max_white;
      break;
    default:
      // Color::UNKNOWN
      break;
  }

  float diff_h = float((max_h - min_h) / 2);
  float avg_h = min_h + diff_h;

  int channels = src_hsv.channels();
  int nRows = src_hsv.rows;

  //图像数据列需要考虑通道数的影响；

  int nCols = src_hsv.cols * channels;

  //连续存储的数据，按一行处理

  if (src_hsv.isContinuous()) {
    nCols *= nRows;
    nRows = 1;
  }

  int i, j;
  uchar* p;
  float s_all = 0;
  float v_all = 0;
  float count = 0;
  for (i = 0; i < nRows; ++i) {
    p = src_hsv.ptr<uchar>(i);
    for (j = 0; j < nCols; j += 3) {
      int H = int(p[j]);      // 0-180
      int S = int(p[j + 1]);  // 0-255
      int V = int(p[j + 2]);  // 0-255

      s_all += S;
      v_all += V;
      count++;

      bool colorMatched = false;

      if (H > min_h && H < max_h) {
        float Hdiff = 0;
        if (H > avg_h)
          Hdiff = H - avg_h;
        else
          Hdiff = avg_h - H;

        float Hdiff_p = float(Hdiff) / diff_h;

        // S和V的最小值由adaptive_minsv这个bool值判断
        // 如果为true，则最小值取决于H值，按比例衰减
        // 如果为false，则不再自适应，使用固定的最小值minabs_sv

        float min_sv = 0;
        if (true == adaptive_minsv)
          min_sv =
              minref_sv -
                  minref_sv / 2 *
                      (1
                          - Hdiff_p);  // inref_sv - minref_sv / 2 * (1 - Hdiff_p)
        else
          min_sv = minabs_sv;  // add

        if ((S > min_sv && S < max_sv) && (V > min_sv && V < max_sv))
          colorMatched = true;
      }

      if (colorMatched == true) {
        p[j] = 0;
        p[j + 1] = 0;
        p[j + 2] = 255;
      } else {
        p[j] = 0;
        p[j + 1] = 0;
        p[j + 2] = 0;
      }
    }
  }

  // cout << "avg_s:" << s_all / count << endl;
  // cout << "avg_v:" << v_all / count << endl;

  // 获取颜色匹配后的二值灰度图

  Mat src_grey;
  std::vector<cv::Mat> hsvSplit_done;
  split(src_hsv, hsvSplit_done);
  src_grey = hsvSplit_done[2];

  match = src_grey;

  return src_grey;
}

bool bFindLeftRightBound1(Mat &bound_threshold, int &posLeft, int &posRight) {

  //从两边寻找边界

  float span = bound_threshold.rows * 0.2f;

  //左边界检测

  for (int i = 0; i < bound_threshold.cols - span - 1; i += 3) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l < i + span; l++) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }
    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.15) {
      posLeft = i;
      break;
    }
  }
  span = bound_threshold.rows * 0.2f;

  //右边界检测

  for (int i = bound_threshold.cols - 1; i > span; i -= 2) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l > i - span; l--) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }

    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.06) {
      posRight = i;
      if (posRight + 5 < bound_threshold.cols) {
        posRight = posRight + 5;
      } else {
        posRight = bound_threshold.cols - 1;
      }

      break;
    }
  }

  if (posLeft < posRight) {
    return true;
  }
  return false;
}

bool bFindLeftRightBound(Mat &bound_threshold, int &posLeft, int &posRight) {

  //从两边寻找边界

  float span = bound_threshold.rows * 0.2f;

  //左边界检测

  for (int i = 0; i < bound_threshold.cols - span - 1; i += 2) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l < i + span; l++) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }
    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.36) {
      posLeft = i;
      break;
    }
  }
  span = bound_threshold.rows * 0.2f;

  //右边界检测

  for (int i = bound_threshold.cols - 1; i > span; i -= 2) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l > i - span; l--) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }

    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.26) {
      posRight = i;
      break;
    }
  }

  if (posLeft < posRight) {
    return true;
  }
  return false;
}

bool bFindLeftRightBound2(Mat &bound_threshold, int &posLeft, int &posRight) {

  //从两边寻找边界

  float span = bound_threshold.rows * 0.2f;

  //左边界检测

  for (int i = 0; i < bound_threshold.cols - span - 1; i += 3) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l < i + span; l++) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }
    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.32) {
      posLeft = i;
      break;
    }
  }
  span = bound_threshold.rows * 0.2f;

  //右边界检测

  for (int i = bound_threshold.cols - 1; i > span; i -= 3) {
    int whiteCount = 0;
    for (int k = 0; k < bound_threshold.rows; k++) {
      for (int l = i; l > i - span; l--) {
        if (bound_threshold.data[k * bound_threshold.step[0] + l] == 255) {
          whiteCount++;
        }
      }
    }

    if (whiteCount * 1.0 / (span * bound_threshold.rows) > 0.22) {
      posRight = i;
      break;
    }
  }

  if (posLeft < posRight) {
    return true;
  }
  return false;
}

//! 判断一个车牌的颜色
//! 输入车牌mat与颜色模板
//! 返回true或fasle

bool plateColorJudge(const Mat &src, const Color r, const bool adaptive_minsv,
                     float &percent) {
  // 判断阈值

  const float thresh = 0.45f;

  Mat src_gray;
  colorMatch(src, src_gray, r, adaptive_minsv);

  percent =
      float(countNonZero(src_gray)) / float(src_gray.rows * src_gray.cols);
  // cout << "percent:" << percent << endl;

  if (percent > thresh)
    return true;
  else
    return false;
}

//判断车牌的类型

Color getPlateType(const Mat &src, const bool adaptive_minsv) {
  float max_percent = 0;
  Color max_color = UNKNOWN;

  float blue_percent = 0;
  float yellow_percent = 0;
  float white_percent = 0;

  if (plateColorJudge(src, BLUE, adaptive_minsv, blue_percent) == true) {
    // cout << "BLUE" << endl;
    return BLUE;
  } else if (plateColorJudge(src, YELLOW, adaptive_minsv, yellow_percent) ==
      true) {
    // cout << "YELLOW" << endl;
    return YELLOW;
  } else if (plateColorJudge(src, WHITE, adaptive_minsv, white_percent) ==
      true) {
    // cout << "WHITE" << endl;
    return WHITE;
  } else {
    // cout << "OTHER" << endl;

    // 如果任意一者都不大于阈值，则取值最大者

    max_percent = blue_percent > yellow_percent ? blue_percent : yellow_percent;
    max_color = blue_percent > yellow_percent ? BLUE : YELLOW;

    max_color = max_percent > white_percent ? max_color : WHITE;
    return max_color;
  }
}

void clearLiuDingOnly(Mat &img) {
  const int x = 7;
  Mat jump = Mat::zeros(1, img.rows, CV_32F);
  for (int i = 0; i < img.rows; i++) {
    int jumpCount = 0;
    int whiteCount = 0;
    for (int j = 0; j < img.cols - 1; j++) {
      if (img.at<char>(i, j) != img.at<char>(i, j + 1)) jumpCount++;

      if (img.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }

    jump.at<float>(i) = (float) jumpCount;
  }

  for (int i = 0; i < img.rows; i++) {
    if (jump.at<float>(i) <= x) {
      for (int j = 0; j < img.cols; j++) {
        img.at<char>(i, j) = 0;
      }
    }
  }
}

//去除车牌上方的钮钉
//计算每行元素的阶跃数，如果小于X认为是柳丁，将此行全部填0（涂黑）
// X的推荐值为，可根据实际调整

bool clearLiuDing(Mat &img) {
  std::vector<float> fJump;
  int whiteCount = 0;
  const int x = 7;
  Mat jump = Mat::zeros(1, img.rows, CV_32F);
  for (int i = 0; i < img.rows; i++) {
    int jumpCount = 0;

    for (int j = 0; j < img.cols - 1; j++) {
      if (img.at<char>(i, j) != img.at<char>(i, j + 1)) jumpCount++;

      if (img.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }

    jump.at<float>(i) = (float) jumpCount;
  }

  int iCount = 0;
  for (int i = 0; i < img.rows; i++) {
    fJump.push_back(jump.at<float>(i));
    if (jump.at<float>(i) >= 16 && jump.at<float>(i) <= 45) {

      //车牌字符满足一定跳变条件

      iCount++;
    }
  }

  ////这样的不是车牌

  if (iCount * 1.0 / img.rows <= 0.40) {

    //满足条件的跳变的行数也要在一定的阈值内

    return false;
  }

  //不满足车牌的条件

  if (whiteCount * 1.0 / (img.rows * img.cols) < 0.15 ||
      whiteCount * 1.0 / (img.rows * img.cols) > 0.50) {
    return false;
  }

  for (int i = 0; i < img.rows; i++) {
    if (jump.at<float>(i) <= x) {
      for (int j = 0; j < img.cols; j++) {
        img.at<char>(i, j) = 0;
      }
    }
  }
  return true;
}

void clearLiuDing(Mat mask, int &top, int &bottom) {
  const int x = 7;

  for (int i = 0; i < mask.rows / 2; i++) {
    int whiteCount = 0;
    int jumpCount = 0;
    for (int j = 0; j < mask.cols - 1; j++) {
      if (mask.at<char>(i, j) != mask.at<char>(i, j + 1)) jumpCount++;

      if ((int) mask.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }
    if ((jumpCount < x && whiteCount * 1.0 / mask.cols > 0.15) ||
        whiteCount < 4) {
      top = i;
    }
  }
  top -= 1;
  if (top < 0) {
    top = 0;
  }

  // ok,找到上下边界

  for (int i = mask.rows - 1; i >= mask.rows / 2; i--) {
    int jumpCount = 0;
    int whiteCount = 0;
    for (int j = 0; j < mask.cols - 1; j++) {
      if (mask.at<char>(i, j) != mask.at<char>(i, j + 1)) jumpCount++;
      if (mask.at<uchar>(i, j) == 255) {
        whiteCount++;
      }
    }
    if ((jumpCount < x && whiteCount * 1.0 / mask.cols > 0.15) ||
        whiteCount < 4) {
      bottom = i;
    }
  }
  bottom += 1;
  if (bottom >= mask.rows) {
    bottom = mask.rows - 1;
  }

  if (top >= bottom) {
    top = 0;
    bottom = mask.rows - 1;
  }
}

int ThresholdOtsu(Mat mat) {
  int height = mat.rows;
  int width = mat.cols;

  // histogram
  float histogram[256] = {0};
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      unsigned char p = (unsigned char) ((mat.data[i * mat.step[0] + j]));
      histogram[p]++;
    }
  }
  // normalize histogram
  int size = height * width;
  for (int i = 0; i < 256; i++) {
    histogram[i] = histogram[i] / size;
  }

  // average pixel value
  float avgValue = 0;
  for (int i = 0; i < 256; i++) {
    avgValue += i * histogram[i];
  }

  int thresholdV;
  float maxVariance = 0;
  float w = 0, u = 0;
  for (int i = 0; i < 256; i++) {
    w += histogram[i];
    u += i * histogram[i];

    float t = avgValue * w - u;
    float variance = t * t / (w * (1 - w));
    if (variance > maxVariance) {
      maxVariance = variance;
      thresholdV = i;
    }
  }

  return thresholdV;
}

//! 直方图均衡

Mat histeq(Mat in) {
  Mat out(in.size(), in.type());
  if (in.channels() == 3) {
    Mat hsv;
    std::vector<cv::Mat> hsvSplit;
    cvtColor(in, hsv, CV_BGR2HSV);
    split(hsv, hsvSplit);
    equalizeHist(hsvSplit[2], hsvSplit[2]);
    merge(hsvSplit, hsv);
    cvtColor(hsv, out, CV_HSV2BGR);
  } else if (in.channels() == 1) {
    equalizeHist(in, out);
  }
  return out;
}

#define HORIZONTAL 1
#define VERTICAL 0

Mat CutTheRect(Mat &in, Rect &rect) {
  int size = in.cols;  // (rect.width>rect.height)?rect.width:rect.height;
  Mat dstMat(size, size, CV_8UC1);
  dstMat.setTo(Scalar(0, 0, 0));

  int x = (int) floor((float) (size - rect.width) / 2.0f);
  int y = (int) floor((float) (size - rect.height) / 2.0f);

  //把rect中的数据 考取到dstMat的中间

  for (int i = 0; i < rect.height; ++i) {

    //宽

    for (int j = 0; j < rect.width; ++j) {
      dstMat.data[dstMat.step[0] * (i + y) + j + x] =
          in.data[in.step[0] * (i + rect.y) + j + rect.x];
    }
  }

  //
  return dstMat;
}

Rect GetCenterRect(Mat &in) {
  Rect _rect;

  int top = 0;
  int bottom = in.rows - 1;

  //上下

  for (int i = 0; i < in.rows; ++i) {
    bool bFind = false;
    for (int j = 0; j < in.cols; ++j) {
      if (in.data[i * in.step[0] + j] > 20) {
        top = i;
        bFind = true;
        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }
  for (int i = in.rows - 1;
  i >= 0;
  --i) {
    bool bFind = false;
    for (int j = 0; j < in.cols; ++j) {
      if (in.data[i * in.step[0] + j] > 20) {
        bottom = i;
        bFind = true;
        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }

  //左右

  int left = 0;
  int right = in.cols - 1;
  for (int j = 0; j < in.cols; ++j) {
    bool bFind = false;
    for (int i = 0; i < in.rows; ++i) {
      if (in.data[i * in.step[0] + j] > 20) {
        left = j;
        bFind = true;
        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }
  for (int j = in.cols - 1;
  j >= 0;
  --j) {
    bool bFind = false;
    for (int i = 0; i < in.rows; ++i) {
      if (in.data[i * in.step[0] + j] > 20) {
        right = j;
        bFind = true;

        break;
      }
    }
    if (bFind) {
      break;
    }

    //统计这一行或一列中，非零元素的个数

  }

  _rect.x = left;
  _rect.y = top;
  _rect.width = right - left + 1;
  _rect.height = bottom - top + 1;

  return _rect;
}

Mat charFeatures(Mat in, int sizeData) {

  //抠取中间区域

  Rect _rect = GetCenterRect(in);

  Mat tmpIn = CutTheRect(in, _rect);
  // Mat tmpIn = in.clone();
  // Low data feature
  Mat lowData;
  resize(tmpIn, lowData, Size(sizeData, sizeData));

  // Histogram features
  Mat vhist = ProjectedHistogram(lowData, VERTICAL);
  Mat hhist = ProjectedHistogram(lowData, HORIZONTAL);

  // Last 10 is the number of moments components
  int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;

  Mat out = Mat::zeros(1, numCols, CV_32F);
  // Asign values to

  // feature,ANN的样本特征为水平、垂直直方图和低分辨率图像所组成的矢量

  int j = 0;
  for (int i = 0; i < vhist.cols; i++) {
    out.at<float>(j) = vhist.at<float>(i);
    j++;
  }
  for (int i = 0; i < hhist.cols; i++) {
    out.at<float>(j) = hhist.at<float>(i);
    j++;
  }
  for (int x = 0; x < lowData.cols; x++) {
    for (int y = 0; y < lowData.rows; y++) {
      out.at<float>(j) += (float) lowData.at <unsigned char> (x, y);
      j++;
    }
  }
  return out;
}

float countOfBigValue(Mat &mat, int iValue) {
  float iCount = 0.0;
  if (mat.rows > 1) {
    for (int i = 0; i < mat.rows; ++i) {
      if (mat.data[i * mat.step[0]] > iValue) {
        iCount += 1.0;
      }
    }
    return iCount;

  } else {
    for (int i = 0; i < mat.cols; ++i) {
      if (mat.data[i] > iValue) {
        iCount += 1.0;
      }
    }

    return iCount;
  }
}

// ！获取垂直和水平方向直方图

Mat ProjectedHistogram(Mat img, int t) {
  int sz = (t) ? img.rows : img.cols;
  Mat mhist = Mat::zeros(1, sz, CV_32F);

  for (int j = 0; j < sz; j++) {
    Mat data = (t) ? img.row(j) : img.col(j);

    //统计这一行或一列中，非零元素的个数，并保存到mhist中

    mhist.at<float>(j) = countOfBigValue(data, 20);
  }

  // Normalize histogram
  double min, max;
  minMaxLoc(mhist, &min, &max);

  //用mhist直方图中的最大值，归一化直方图

  if (max > 0)
    mhist.convertTo(mhist, -1, 1.0f / max, 0);

  return mhist;
}

Mat preprocessChar(Mat in, int char_size) {
  // Remap image
  int h = in.rows;
  int w = in.cols;

  //统一每个字符的大小

  int charSize = char_size;

  Mat transformMat = Mat::eye(2, 3, CV_32F);
  int m = max(w, h);
  transformMat.at<float>(0, 2) = float(m / 2 - w / 2);
  transformMat.at<float>(1, 2) = float(m / 2 - h / 2);

  Mat warpImage(m, m, in.type());
  warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR,
    BORDER_CONSTANT, Scalar(0));

  //！ 将所有的字符调整成统一的尺寸

  Mat out;
  resize(warpImage, out, Size(charSize, charSize));

  return out;
}

Rect GetChineseRect(const Rect rectSpe) {
  int height = rectSpe.height;
  float newwidth = rectSpe.width * 1.10f;
  int x = rectSpe.x;
  int y = rectSpe.y;

  int newx = x - int(newwidth * 1.10f);
  newx = newx > 0 ? newx : 0;

  Rect a(newx, y, int(newwidth), height);

  return a;
}

bool verifyCharSizes(Rect r) {
  // Char sizes 45x90
  float aspect = 45.0f / 90.0f;
  float charAspect = (float)r.width / (float)r.height;
  float error = 0.35f;
  float minHeight = 25.f;
  float maxHeight = 50.f;
  // We have a different aspect ratio for number 1, and it can be ~0.2
  float minAspect = 0.05f;
  float maxAspect = aspect + aspect * error;

  // bb area
  int bbArea = r.width * r.height;

  if (charAspect > minAspect && charAspect < maxAspect /*&&
                                                       r.rows >= minHeight && r.rows < maxHeight*/)
                                                       return true;
  else
    return false;
}

// 图像缩放
Mat scaleImage(const Mat& image, const Size& maxSize, double& scale_ratio) {
  Mat ret;

  if (image.cols > maxSize.width || image.rows > maxSize.height) {
    double widthRatio = image.cols / (double)maxSize.width;
    double heightRatio = image.rows / (double)maxSize.height;
    double m_real_to_scaled_ratio = max(widthRatio, heightRatio);

    int newWidth = int(image.cols / m_real_to_scaled_ratio);
    int newHeight = int(image.rows / m_real_to_scaled_ratio);

    resize(image, ret, Size(newWidth, newHeight), 0, 0);
    scale_ratio = m_real_to_scaled_ratio;
  }
  else {
    ret = image;
    scale_ratio = 1.0;
  }

  return ret;
}

bool verifyPlateSize(Rect mr) {
  float error = 0.6f;
  // Spain car plate size: 52x11 aspect 4,7272
  // China car plate size: 440mm*140mm，aspect 3.142857

  // Real car plate size: 136 * 32, aspect 4
  float aspect = 3.75;

  // Set a min and max area. All other patchs are discarded
  // int min= 1*aspect*1; // minimum area
  // int max= 2000*aspect*2000; // maximum area
  int min = 34 * 8 * 1;  // minimum area
  int max = 34 * 8 * 200;  // maximum area

  // Get only patchs that match to a respect ratio.
  float rmin = aspect - aspect * error;
  float rmax = aspect + aspect * error;

  float area = float(mr.height * mr.width);
  float r = (float)mr.width / (float)mr.height;
  if (r < 1) r = (float)mr.height / (float)mr.width;

  // cout << "area:" << area << endl;
  // cout << "r:" << r << endl;

  if ((area < min || area > max) || (r < rmin || r > rmax))
    return false;
  else
    return true;
}

bool verifyRotatedPlateSizes(RotatedRect mr) {
  float error = 0.5f;
  // Spain car plate size: 52x11 aspect 4,7272
  // China car plate size: 440mm*140mm，aspect 3.142857

  // Real car plate size: 136 * 32, aspect 4
  float aspect = 3.75f;

  // Set a min and max area. All other patchs are discarded
  // int min= 1*aspect*1; // minimum area
  // int max= 2000*aspect*2000; // maximum area
  //int min = 34 * 8 * 1;  // minimum area
  //int max = 34 * 8 * 200;  // maximum area

  // Get only patchs that match to a respect ratio.
  float aspect_min = aspect - aspect * error;
  float aspect_max = aspect + aspect * error * 0.5f;

  float width_max = 400.f;
  float width_min = 40.f;

  float min = float(width_min * width_min / aspect_max);  // minimum area
  float max = float(width_max * width_max / aspect_min);  // maximum area

  float width = mr.size.width;
  float height = mr.size.height;
  float area = width * height;

  float ratio = width / height;
  float angle = mr.angle;
  if (ratio < 1) {
    swap(width, height);
    ratio = width / height;

    angle = 90.f + angle;
    //std::cout << "angle:" << angle << std::endl;
  }

  float angle_min = -60.f;
  float angle_max = 60.f;

  //std::cout << "area:" << area << std::endl;
  //std::cout << "ratio:" << ratio << std::endl;
  //std::cout << "angle:" << angle << std::endl;

  if ((area < min || area > max) || (ratio < aspect_min || ratio > aspect_max)
    || (angle < angle_min || angle > angle_max) || (width < width_min || width > width_max)) {

    //if (abs(angle) - 0.f < 3.f ) {
    //  std::cout << "min:" << min << std::endl;
    //  std::cout << "max:" << max << std::endl;
    //  std::cout << "area:" << area << std::endl;
    //  std::cout << "ratio:" << ratio << std::endl;
    //  std::cout << "angle:" << angle << std::endl;
    //  return true;
    //}     
    return false;
  }
  else
    return true;
}

//! use verify size to first generate candidates
Mat mserMatch(const Mat &src, Mat &match, const Color r,
  std::vector<RotatedRect>& out_plateRect, std::vector<Rect>& out_charRect) {
  Mat image = src;
 
  std::vector<RotatedRect> plateRects;
  std::vector<Rect> charRects;

  std::vector<std::vector<Point>> all_contours;
  std::vector<Rect> all_boxes;

    Ptr<MSER> mser;
    std::vector<CCharacter> charVec;
  
    match = Mat::zeros(image.rows, image.cols, image.type());

    Mat result = image.clone();
    cvtColor(result, result, COLOR_GRAY2BGR);

    int imageArea = image.rows * image.cols;
    mser = MSER::create(1, 30, int(0.05 * imageArea));
    mser->detectRegions(image, all_contours, all_boxes);

    size_t size = all_contours.size();

    int char_index = 0;

    for (size_t index = 0; index < size; index++) {
      Rect rect = all_boxes[index];
      std::vector<Point> contour = all_contours[index];
      RotatedRect rrect = minAreaRect(Mat(contour));

      if (verifyRotatedPlateSizes(rrect)) {
        //cv::rectangle(result, rrect.boundingRect(), Scalar(0, 255, 0));

        Point2f rect_points[4];
        rrect.points(rect_points);
        for (int j = 0; j < 4; j++)
          line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 1, 8);

        plateRects.push_back(rrect);
      }

      if (verifyCharSizes(rect)) {
        //if (0)
        //{
        //  std::stringstream ss(std::stringstream::in | std::stringstream::out);
        //  ss << "resources/image/tmp/character" << char_index++ << ".jpg";
        //  imwrite(ss.str(), mserMat);
        //}

        if (1) {
          //match(rect) = min(max(0, int(maxVal * 255)),255);
          match(rect) = 255;

          cv::rectangle(result, rect, Scalar(255, 0, 0));
          Point center(rect.tl().x + rect.width / 2, rect.tl().y + rect.height / 2);

          //cv::circle(result, center, 3, Scalar(0, 255, 0), 2);

          charRects.push_back(rect);
          out_charRect.push_back(rect);
          //CCharacter character;
          //character.setCharacterPos(rect);
          //character.setCharacterMat(binary_region);
          //character.setCharacterStr(label);
          //character.setCharacterScore(maxVal);
          //charVec.push_back(character);
        }
      }
    }
 
    for (auto prect : plateRects) {
      float areasum = 0.f;
      int count = 0;

      Rect boundingrect = prect.boundingRect();
      for (auto crect : charRects) {
        
        Rect interRect = boundingrect & crect;
        if (interRect == crect) {
 
          Mat region = image(crect);
          Mat binary_region;
          threshold(region, binary_region, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

          Mat charInput = preprocessChar(binary_region, 20);

          	if(0)
          	{ 
          		std::stringstream ss(std::stringstream::in | std::stringstream::out);
              ss << "resources/image/tmp/character" << char_index++ << ".jpg";
              imwrite(ss.str(), charInput);
          	}

          std::string label = "";
          float maxVal = -2.f;
          bool isCharacter = CharsIdentify::instance()->isCharacter(charInput, label, maxVal);
          if (isCharacter) {
            areasum += crect.area();
            count++;
          }
        }
      }

      float ratio = areasum / (float)boundingrect.area();
      if (ratio - 0.5f > 0 && count >= 5) {
        std::cout << "ratio:" << ratio << std::endl;
        std::cout << "count:" << count << std::endl;
        //cv::rectangle(result, prect.boundingRect(), Scalar(0, 255, 0));
        out_plateRect.push_back(prect);
      }
    }

    if (1) {
      imshow("result", result);
      waitKey(0);
    }

 
  return match;
}


bool mat_valid_position(const Mat& mat, int row, int col) {
  return row >= 0 && col >= 0 && row < mat.rows && col < mat.cols;
}


template<class T>
static void mat_set_invoke(Mat& mat, int row, int col, const Scalar& value) {
  if (1 == mat.channels()) {
    mat.at<T>(row, col) = (T)value.val[0];
  }
  else if (3 == mat.channels()) {
    T* ptr_src = mat.ptr<T>(row, col);
    *ptr_src++ = (T)value.val[0];
    *ptr_src++ = (T)value.val[1];
    *ptr_src = (T)value.val[2];
  }
  else if (4 == mat.channels()) {
    T* ptr_src = mat.ptr<T>(row, col);
    *ptr_src++ = (T)value.val[0];
    *ptr_src++ = (T)value.val[1];
    *ptr_src++ = (T)value.val[2];
    *ptr_src = (T)value.val[3];
  }
}

void setPoint(Mat& mat, int row, int col, const Scalar& value) {
  if (CV_8U == mat.depth()) {
    mat_set_invoke<uchar>(mat, row, col, value);
  }
  else if (CV_8S == mat.depth()) {
    mat_set_invoke<char>(mat, row, col, value);
  }
  else if (CV_16U == mat.depth()) {
    mat_set_invoke<ushort>(mat, row, col, value);
  }
  else if (CV_16S == mat.depth()) {
    mat_set_invoke<short>(mat, row, col, value);
  }
  else if (CV_32S == mat.depth()) {
    mat_set_invoke<int>(mat, row, col, value);
  }
  else if (CV_32F == mat.depth()) {
    mat_set_invoke<float>(mat, row, col, value);
  }
  else if (CV_64F == mat.depth()) {
    mat_set_invoke<double>(mat, row, col, value);
  }
}


Mat adaptive_image_from_points(const std::vector<Point>& points,
  const Rect& rect, const Size& size, const Scalar& backgroundColor /* = ml_color_white */, 
  const Scalar& forgroundColor /* = ml_color_black */, bool gray /* = true */) {
  int expendHeight = 0;
  int expendWidth = 0;

  if (rect.width > rect.height) {
    expendHeight = (rect.width - rect.height) / 2;
  }
  else if (rect.height > rect.width) {
    expendWidth = (rect.height - rect.width) / 2;
  }

  Mat image(rect.height + expendHeight * 2, rect.width + expendWidth * 2, gray ? CV_8UC1 : CV_8UC3, backgroundColor);

  for (int i = 0; i < (int)points.size(); ++i) {
    Point point = points[i];
    Point currentPt(point.x - rect.tl().x + expendWidth, point.y - rect.tl().y + expendHeight);
    if (mat_valid_position(image, currentPt.y, currentPt.x)) {
      setPoint(image, currentPt.y, currentPt.x, forgroundColor);
    }
  }

  Mat result;
  resize(image, result, size, 0, 0, INTER_NEAREST);

  return result;
}



}