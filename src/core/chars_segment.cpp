#include "easypr/core/chars_segment.h"

using namespace std;

namespace easypr {

const float DEFAULT_BLUEPERCEMT = 0.3f;
const float DEFAULT_WHITEPERCEMT = 0.1f;

CCharsSegment::CCharsSegment() {
  m_LiuDingSize = DEFAULT_LIUDING_SIZE;
  m_theMatWidth = DEFAULT_MAT_WIDTH;

  //！车牌颜色判断参数

  m_ColorThreshold = DEFAULT_COLORTHRESHOLD;
  m_BluePercent = DEFAULT_BLUEPERCEMT;
  m_WhitePercent = DEFAULT_WHITEPERCEMT;

  m_debug = DEFAULT_DEBUG;
}

//! 字符尺寸验证

bool CCharsSegment::verifyCharSizes(Mat r) {
  // Char sizes 45x90
  float aspect = 45.0f / 90.0f;
  float charAspect = (float)r.cols / (float)r.rows;
  float error = 0.7f;
  float minHeight = 10.f;
  float maxHeight = 35.f;
  // We have a different aspect ratio for number 1, and it can be ~0.2
  float minAspect = 0.05f;
  float maxAspect = aspect + aspect * error;
  // area of pixels
  int area = cv::countNonZero(r);
  // bb area
  int bbArea = r.cols * r.rows;
  //% of pixel in area
  int percPixels = area / bbArea;

  if (percPixels <= 1 && charAspect > minAspect && charAspect < maxAspect &&
      r.rows >= minHeight && r.rows < maxHeight)
    return true;
  else
    return false;
}

//! 字符预处理

Mat CCharsSegment::preprocessChar(Mat in) {
  // Remap image
  int h = in.rows;
  int w = in.cols;

  //统一每个字符的大小

  int charSize = CHAR_SIZE;

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

//! 字符分割与排序

int CCharsSegment::charsSegment(Mat input, vector<Mat>& resultVec) {
  if (!input.data) return 0x01;

  int w = input.cols;
  int h = input.rows;

  Mat tmpMat = input(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));

  // 判断车牌颜色以此确认threshold方法

  Color plateType = getPlateType(tmpMat, true);

  Mat input_grey;
  cvtColor(input, input_grey, CV_BGR2GRAY);

  Mat img_threshold;

  // 二值化
  // 根据车牌的不同颜色使用不同的阈值判断方法
  // TODO：使用MSER来提取这些轮廓

  if (BLUE == plateType) {
    // cout << "BLUE" << endl;
    img_threshold = input_grey.clone();

    int w = input_grey.cols;
    int h = input_grey.rows;
    Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
    int threadHoldV = ThresholdOtsu(tmp);

    threshold(input_grey, img_threshold, threadHoldV, 255, CV_THRESH_BINARY);

  } else if (YELLOW == plateType) {
    // cout << "YELLOW" << endl;
    img_threshold = input_grey.clone();
    int w = input_grey.cols;
    int h = input_grey.rows;
    Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
    int threadHoldV = ThresholdOtsu(tmp);
    // utils::imwrite("resources/image/tmp/inputgray2.jpg", input_grey);

    threshold(input_grey, img_threshold, threadHoldV, 255,
              CV_THRESH_BINARY_INV);

  } else if (WHITE == plateType) {
    // cout << "WHITE" << endl;

    threshold(input_grey, img_threshold, 10, 255,
              CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
  } else {
    // cout << "UNKNOWN" << endl;
    threshold(input_grey, img_threshold, 10, 255,
              CV_THRESH_OTSU + CV_THRESH_BINARY);
  }

  // 去除车牌上方的柳钉以及下方的横线等干扰
  // 并且也判断了是否是车牌
  // 并且在此对字符的跳变次数以及字符颜色所占的比重做了是否是车牌的判别条件
  // 如果不是车牌，返回ErrorCode=0x02

  if (!clearLiuDing(img_threshold)) return 0x02;

  // 在二值化图像中提取轮廓

  Mat img_contours;
  img_threshold.copyTo(img_contours);

  vector<vector<Point> > contours;
  findContours(img_contours,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // retrieve the external contours
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();
  vector<Rect> vecRect;

  // 将不符合特定尺寸的字符块排除出去

  while (itc != contours.end()) {
    Rect mr = boundingRect(Mat(*itc));
    Mat auxRoi(img_threshold, mr);

    if (verifyCharSizes(auxRoi)) vecRect.push_back(mr);
    ++itc;
  }

  // 如果找不到任何字符块，则返回ErrorCode=0x03

  if (vecRect.size() == 0) return 0x03;

  // 对符合尺寸的图块按照从左到右进行排序;
  // 直接使用stl的sort方法，更有效率

  vector<Rect> sortedRect(vecRect);
  std::sort(sortedRect.begin(), sortedRect.end(),
            [](const Rect& r1, const Rect& r2) { return r1.x < r2.x; });

  size_t specIndex = 0;

  // 获得特殊字符对应的Rectt,如苏A的"A"

  specIndex = GetSpecificRect(sortedRect);

  // 根据特定Rect向左反推出中文字符
  // 这样做的主要原因是根据findContours方法很难捕捉到中文字符的准确Rect，因此仅能
  // 退过特定算法来指定

  Rect chineseRect;
  if (specIndex < sortedRect.size())
    chineseRect = GetChineseRect(sortedRect[specIndex]);
  else
    return 0x04;

  //新建一个全新的排序Rect
  //将中文字符Rect第一个加进来，因为它肯定是最左边的
  //其余的Rect只按照顺序去6个，车牌只可能是7个字符！这样可以避免阴影导致的“1”字符

  vector<Rect> newSortedRect;
  newSortedRect.push_back(chineseRect);
  RebuildRect(sortedRect, newSortedRect, specIndex);

  if (newSortedRect.size() == 0) return 0x05;

  // 开始截取每个字符

  for (size_t i = 0; i < newSortedRect.size(); i++) {
    Rect mr = newSortedRect[i];

    // Mat auxRoi(img_threshold, mr);

    // 使用灰度图来截取图块，然后依次对每个图块进行大津阈值来二值化

    Mat auxRoi(input_grey, mr);
    Mat newRoi;

    if (BLUE == plateType) {
      /* img_threshold = auxRoi.clone();
       int w = input_grey.cols;
       int h = input_grey.rows;
       Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
       int threadHoldV = ThresholdOtsu(tmp);*/

      threshold(auxRoi, newRoi, 5, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    } else if (YELLOW == plateType) {
      threshold(auxRoi, newRoi, 5, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);

    } else if (WHITE == plateType) {
      threshold(auxRoi, newRoi, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
    } else {
      threshold(auxRoi, newRoi, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
    }

    // 归一化大小

    newRoi = preprocessChar(newRoi);

    // 每个字符图块输入到下面的步骤进行处理

    resultVec.push_back(newRoi);
  }

  return 0;
}

//! 根据特殊车牌来构造猜测中文字符的位置和大小

Rect CCharsSegment::GetChineseRect(const Rect rectSpe) {
  int height = rectSpe.height;
  float newwidth = rectSpe.width * 1.15f;
  int x = rectSpe.x;
  int y = rectSpe.y;

  int newx = x - int(newwidth * 1.15);
  newx = newx > 0 ? newx : 0;

  Rect a(newx, y, int(newwidth), height);

  return a;
}

//! 找出指示城市的字符的Rect，例如苏A7003X，就是"A"的位置

int CCharsSegment::GetSpecificRect(const vector<Rect>& vecRect) {
  vector<int> xpositions;
  int maxHeight = 0;
  int maxWidth = 0;

  for (size_t i = 0; i < vecRect.size(); i++) {
    xpositions.push_back(vecRect[i].x);

    if (vecRect[i].height > maxHeight) {
      maxHeight = vecRect[i].height;
    }
    if (vecRect[i].width > maxWidth) {
      maxWidth = vecRect[i].width;
    }
  }

  int specIndex = 0;
  for (size_t i = 0; i < vecRect.size(); i++) {
    Rect mr = vecRect[i];
    int midx = mr.x + mr.width / 2;

    //如果一个字符有一定的大小，并且在整个车牌的1/7到2/7之间，则是我们要找的特殊字符
    //当前字符和下个字符的距离在一定的范围内

    if ((mr.width > maxWidth * 0.8 || mr.height > maxHeight * 0.8) &&
        (midx < int(m_theMatWidth / 7) * 2 &&
         midx > int(m_theMatWidth / 7) * 1)) {
      specIndex = i;
    }
  }

  return specIndex;
}

//! 这个函数做两个事情
//  1.把特殊字符Rect左边的全部Rect去掉，后面再重建中文字符的位置。
//  2.从特殊字符Rect开始，依次选择6个Rect，多余的舍去。

int CCharsSegment::RebuildRect(const vector<Rect>& vecRect,
                               vector<Rect>& outRect, int specIndex) {
  int count = 6;
  for (size_t i = specIndex; i < vecRect.size() && count; ++i, --count) {
    outRect.push_back(vecRect[i]);
  }

  return 0;
}

}
