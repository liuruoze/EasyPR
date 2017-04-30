#include "easypr/core/chars_segment.h"
#include "easypr/core/chars_identify.h"
#include "easypr/core/core_func.h"
#include "easypr/core/params.h"
#include "easypr/config.h"
#include "thirdparty/mser/mser2.hpp"

namespace easypr {

const float DEFAULT_BLUEPERCEMT = 0.3f;
const float DEFAULT_WHITEPERCEMT = 0.1f;

CCharsSegment::CCharsSegment() {
  m_LiuDingSize = DEFAULT_LIUDING_SIZE;
  m_theMatWidth = DEFAULT_MAT_WIDTH;

  m_ColorThreshold = DEFAULT_COLORTHRESHOLD;
  m_BluePercent = DEFAULT_BLUEPERCEMT;
  m_WhitePercent = DEFAULT_WHITEPERCEMT;

  m_debug = DEFAULT_DEBUG;
}


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


Mat CCharsSegment::preprocessChar(Mat in) {
  // Remap image
  int h = in.rows;
  int w = in.cols;

  int charSize = CHAR_SIZE;

  Mat transformMat = Mat::eye(2, 3, CV_32F);
  int m = max(w, h);
  transformMat.at<float>(0, 2) = float(m / 2 - w / 2);
  transformMat.at<float>(1, 2) = float(m / 2 - h / 2);

  Mat warpImage(m, m, in.type());
  warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR,
             BORDER_CONSTANT, Scalar(0));

  Mat out;
  resize(warpImage, out, Size(charSize, charSize));

  return out;
}


//! choose the bese threshold method for chinese
void CCharsSegment::judgeChinese(Mat in, Mat& out, Color plateType) {
  Mat auxRoi = in;
  float valOstu = -1.f, valAdap = -1.f;
  Mat roiOstu, roiAdap;
  bool isChinese = true;
  if (1) {
    if (BLUE == plateType) {
      threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
    }
    else if (YELLOW == plateType) {
      threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
    }
    else if (WHITE == plateType) {
      threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
    }
    else {
      threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
    }
    roiOstu = preprocessChar(roiOstu);
    if (0) {
      imshow("roiOstu", roiOstu);
      waitKey(0);
      destroyWindow("roiOstu");
    }
    auto character = CharsIdentify::instance()->identifyChinese(roiOstu, valOstu, isChinese);
  }
  if (1) {
    if (BLUE == plateType) {
      adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 0);
    }
    else if (YELLOW == plateType) {
      adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 0);
    }
    else if (WHITE == plateType) {
      adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 0);
    }
    else {
      adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 0);
    }
    roiAdap = preprocessChar(roiAdap);
    auto character = CharsIdentify::instance()->identifyChinese(roiAdap, valAdap, isChinese);
  }

  //std::cout << "valOstu: " << valOstu << std::endl;
  //std::cout << "valAdap: " << valAdap << std::endl;

  if (valOstu >= valAdap) {
    out = roiOstu;
  }
  else {
    out = roiAdap;
  }
}

void CCharsSegment::judgeChineseGray(Mat in, Mat& out, Color plateType) {
  out = in;
}

bool slideChineseWindow(Mat& image, Rect mr, Mat& newRoi, Color plateType, float slideLengthRatio, bool useAdapThreshold) {
  std::vector<CCharacter> charCandidateVec;

  Rect maxrect = mr;
  Point tlPoint = mr.tl();

  bool isChinese = true;
  int slideLength = int(slideLengthRatio * maxrect.width);
  int slideStep = 1;
  int fromX = 0;
  fromX = tlPoint.x;

  for (int slideX = -slideLength; slideX < slideLength; slideX += slideStep) {
    float x_slide = 0;

    x_slide = float(fromX + slideX);

    float y_slide = (float)tlPoint.y;
    Point2f p_slide(x_slide, y_slide);

    //cv::circle(image, p_slide, 2, Scalar(255), 1);

    int chineseWidth = int(maxrect.width);
    int chineseHeight = int(maxrect.height);

    Rect rect(Point2f(x_slide, y_slide), Size(chineseWidth, chineseHeight));

    if (rect.tl().x < 0 || rect.tl().y < 0 || rect.br().x >= image.cols || rect.br().y >= image.rows)
      continue;

    Mat auxRoi = image(rect);

    Mat roiOstu, roiAdap;
    if (1) {
      if (BLUE == plateType) {
        threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
      }
      else if (YELLOW == plateType) {
        threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
      }
      else if (WHITE == plateType) {
        threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
      }
      else {
        threshold(auxRoi, roiOstu, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
      }
      roiOstu = preprocessChar(roiOstu, kChineseSize);

      CCharacter charCandidateOstu;
      charCandidateOstu.setCharacterPos(rect);
      charCandidateOstu.setCharacterMat(roiOstu);
      charCandidateOstu.setIsChinese(isChinese);
      charCandidateVec.push_back(charCandidateOstu);
    }
    if (useAdapThreshold) {
      if (BLUE == plateType) {
        adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 0);
      }
      else if (YELLOW == plateType) {
        adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 0);
      }
      else if (WHITE == plateType) {
        adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 0);
      }
      else {
        adaptiveThreshold(auxRoi, roiAdap, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, 0);
      }
      roiAdap = preprocessChar(roiAdap, kChineseSize);

      CCharacter charCandidateAdap;
      charCandidateAdap.setCharacterPos(rect);
      charCandidateAdap.setCharacterMat(roiAdap);
      charCandidateAdap.setIsChinese(isChinese);
      charCandidateVec.push_back(charCandidateAdap);
    }

  }

  CharsIdentify::instance()->classifyChinese(charCandidateVec);

  double overlapThresh = 0.1;
  NMStoCharacter(charCandidateVec, overlapThresh);

  if (charCandidateVec.size() >= 1) {
    std::sort(charCandidateVec.begin(), charCandidateVec.end(),
              [](const CCharacter& r1, const CCharacter& r2) {
                return r1.getCharacterScore() > r2.getCharacterScore();
              });

    newRoi = charCandidateVec.at(0).getCharacterMat();
    return true;
  }

  return false;
}

bool slideChineseGrayWindow(const Mat& image, Rect& mr, Mat& newRoi, Color plateType, float slideLengthRatio) {
  std::vector<CCharacter> charCandidateVec;

  Rect maxrect = mr;
  Point tlPoint = mr.tl();

  bool isChinese = true;
  int slideLength = int(slideLengthRatio * maxrect.width);
  int slideStep = 1;
  int fromX = 0;
  fromX = tlPoint.x;

  for (int slideX = -slideLength; slideX < slideLength; slideX += slideStep) {
    float x_slide = 0;
    x_slide = float(fromX + slideX);

    float y_slide = (float)tlPoint.y;

    int chineseWidth = int(maxrect.width);
    int chineseHeight = int(maxrect.height);

    Rect rect(Point2f(x_slide, y_slide), Size(chineseWidth, chineseHeight));

    if (rect.tl().x < 0 || rect.tl().y < 0 || rect.br().x >= image.cols || rect.br().y >= image.rows)
      continue;

    Mat auxRoi = image(rect);
    Mat grayChinese;
    grayChinese.create(kGrayCharHeight, kGrayCharWidth, CV_8UC1);
    resize(auxRoi, grayChinese, grayChinese.size(), 0, 0, INTER_LINEAR);

    CCharacter charCandidateOstu;
    charCandidateOstu.setCharacterPos(rect);
    charCandidateOstu.setCharacterMat(grayChinese);
    charCandidateOstu.setIsChinese(isChinese);
    charCandidateVec.push_back(charCandidateOstu);
  }

  CharsIdentify::instance()->classifyChineseGray(charCandidateVec);

  double overlapThresh = 0.1;
  NMStoCharacter(charCandidateVec, overlapThresh);

  if (charCandidateVec.size() >= 1) {
    std::sort(charCandidateVec.begin(), charCandidateVec.end(),
              [](const CCharacter& r1, const CCharacter& r2) {
                return r1.getCharacterScore() > r2.getCharacterScore();
              });

    newRoi = charCandidateVec.at(0).getCharacterMat();
    mr = charCandidateVec.at(0).getCharacterPos();
    return true;
  }
  return false;
}


int CCharsSegment::charsSegment(Mat input, vector<Mat>& resultVec, Color color) {
  if (!input.data) return 0x01;

  Color plateType = color;

  Mat input_grey;
  cvtColor(input, input_grey, CV_BGR2GRAY);

  if (0) {
    imshow("plate", input_grey);
    waitKey(0);
    destroyWindow("plate");
  }

  Mat img_threshold;

  img_threshold = input_grey.clone();
  spatial_ostu(img_threshold, 8, 2, plateType);

  if (0) {
    imshow("plate", img_threshold);
    waitKey(0);
    destroyWindow("plate");
  }

  // remove liuding and hor lines
  // also judge weather is plate use jump count
  if (!clearLiuDing(img_threshold)) return 0x02;

  Mat img_contours;
  img_threshold.copyTo(img_contours);

  vector<vector<Point> > contours;
  findContours(img_contours,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // retrieve the external contours
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();
  vector<Rect> vecRect;

  while (itc != contours.end()) {
    Rect mr = boundingRect(Mat(*itc));
    Mat auxRoi(img_threshold, mr);

    if (verifyCharSizes(auxRoi)) vecRect.push_back(mr);
    ++itc;
  }


  if (vecRect.size() == 0) return 0x03;

  vector<Rect> sortedRect(vecRect);
  std::sort(sortedRect.begin(), sortedRect.end(),
            [](const Rect& r1, const Rect& r2) { return r1.x < r2.x; });

  size_t specIndex = 0;

  specIndex = GetSpecificRect(sortedRect);

  Rect chineseRect;
  if (specIndex < sortedRect.size())
    chineseRect = GetChineseRect(sortedRect[specIndex]);
  else
    return 0x04;

  if (0) {
    rectangle(img_threshold, chineseRect, Scalar(255));
    imshow("plate", img_threshold);
    waitKey(0);
    destroyWindow("plate");
  }

  vector<Rect> newSortedRect;
  newSortedRect.push_back(chineseRect);
  RebuildRect(sortedRect, newSortedRect, specIndex);

  if (newSortedRect.size() == 0) return 0x05;

  bool useSlideWindow = true;
  bool useAdapThreshold = true;
  //bool useAdapThreshold = CParams::instance()->getParam1b();

  for (size_t i = 0; i < newSortedRect.size(); i++) {
    Rect mr = newSortedRect[i];

    // Mat auxRoi(img_threshold, mr);
    Mat auxRoi(input_grey, mr);
    Mat newRoi;

    if (i == 0) {
      if (useSlideWindow) {
        float slideLengthRatio = 0.1f;
        //float slideLengthRatio = CParams::instance()->getParam1f();
        if (!slideChineseWindow(input_grey, mr, newRoi, plateType, slideLengthRatio, useAdapThreshold))
          judgeChinese(auxRoi, newRoi, plateType);
      }
      else
        judgeChinese(auxRoi, newRoi, plateType);
    }
    else {
      if (BLUE == plateType) {
        threshold(auxRoi, newRoi, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU);
      }
      else if (YELLOW == plateType) {
        threshold(auxRoi, newRoi, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);
      }
      else if (WHITE == plateType) {
        threshold(auxRoi, newRoi, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
      }
      else {
        threshold(auxRoi, newRoi, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
      }

      newRoi = preprocessChar(newRoi);
    }

    if (0) {
      if (i == 0) {
        imshow("input_grey", input_grey);
        waitKey(0);
        destroyWindow("input_grey");
      }
      if (i == 0) {
        imshow("newRoi", newRoi);
        waitKey(0);
        destroyWindow("newRoi");
      }
    }

    resultVec.push_back(newRoi);
  }

  return 0;
}

int CCharsSegment::projectSegment(const Mat& input, Color color, vector<int>& out_indexs) {
  if (!input.data) return 0x01;

  Color plateType = color;
  Mat input_grey;
  cvtColor(input, input_grey, CV_BGR2GRAY);
  SHOW_IMAGE(input_grey, 0);

  Mat img_threshold;
  img_threshold = input_grey.clone();
  spatial_ostu(img_threshold, 8, 2, plateType);
  SHOW_IMAGE(img_threshold, 0);

  // remove liuding and hor lines
  // also judge weather is plate use jump count
  if (!clearLiuDing(img_threshold)) return 0x02;
  SHOW_IMAGE(img_threshold, 0);

  Mat vhist = ProjectedHistogram(img_threshold, VERTICAL, 0);
  Mat showHist = showHistogram(vhist);
  SHOW_IMAGE(showHist, 1);

  vector<float> values;
  vector<int> indexs;
  int size = vhist.cols;
  for (int i = 0; i < size; i++) {
    float val = vhist.at<float>(i);
    values.push_back(1.f - val);
  }
  Mat img_test = img_threshold.clone();
  NMSfor1D<float>(values, indexs);

  out_indexs.resize(size);
  for (int j = 0; j < size; j++)
    out_indexs.at(j) = 0;
  for (int i = 0; i < size; i++) {
    float val = vhist.at<float>(i);
    if (indexs.at(i) && val < 0.1f) {
      out_indexs.at(i) = 1;
      for (int j = 0; j < img_test.rows; j++) {
        img_test.at<char>(j, i) = (char)255;
      }
    }
  }
  SHOW_IMAGE(img_test, 1);

  return 0;
}

bool verifyCharRectSizes(Rect r) {
  // Char sizes 45x90
  float aspect = 45.0f / 90.0f;
  float charAspect = (float)r.width / (float)r.height;
  float error = 0.5f;
  float minHeight = kPlateResizeHeight * 0.5f;
  float maxHeight = kPlateResizeHeight * 1.f;
  // We have a different aspect ratio for number 1, and it can be ~0.2
  float minAspect = 0.10f; //0.2f;

  float maxAspect = 0.85f; // aspect + aspect * error; //0.8f;

  int ch = r.tl().y + r.height / 2;
  int min_ch = int(kPlateResizeHeight * 0.3f);
  int max_ch = int(kPlateResizeHeight * 0.7f);
  if (ch > max_ch || ch < min_ch)
   return false;

  float h = (float)r.height;
  if (h > maxHeight || h < minHeight)
    return false;
  if (charAspect < minAspect || charAspect > maxAspect)
    return false;

  return true;
}

Mat preprocessCharMat(Mat in, int char_size) {
  // Remap image
  int h = in.rows;
  int w = in.cols;

  int charSize = char_size;

  Mat transformMat = Mat::eye(2, 3, CV_32F);
  int m = max(w, h);
  transformMat.at<float>(0, 2) = float(m / 2 - w / 2);
  transformMat.at<float>(1, 2) = float(m / 2 - h / 2);

  Mat warpImage(m, m, in.type());
  warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR,
             BORDER_CONSTANT, Scalar(0));

  Mat out;
  cv::resize(warpImage, out, Size(charSize, charSize));

  return out;
}

Mat clearLiuDingAndBorder(const Mat& grayImage, Color color) {
  SHOW_IMAGE(grayImage, 0);
  Mat img_threshold;
  img_threshold = grayImage.clone();
  spatial_ostu(img_threshold, 1, 1, color);
  clearLiuDing(img_threshold);
  Rect cropRect;
  clearBorder(img_threshold, cropRect);
  Mat cropedGrayImage;
  resize(grayImage(cropRect), cropedGrayImage, Size(kPlateResizeWidth, kPlateResizeHeight));
  SHOW_IMAGE(cropedGrayImage, 0);
  return cropedGrayImage;
}

void NMStoCharacterByRatio(std::vector<CCharacter> &inVec, double overlap, const Rect groundRect) {
  // rechange the score
  for (auto& character : inVec) {
    double score = character.getCharacterScore();
    //cout << "score:" << score << endl;
    Rect rect = character.getCharacterPos();
    int w = rect.width;
    int h = rect.height;
    int gw = groundRect.width;
    int gh = groundRect.height;

    float iou = computeIOU(rect, groundRect);

    int w_diff = abs(w - gw);
    int h_diff = abs(h - gh);

    //float w_ratio = (float)w / (float)gw;
    //float h_ratio = (float)h / (float)gh;

    float w_ratio = 1 - (float)w_diff / (float)gw;
    float h_ratio = 1 - (float)h_diff / (float)gh;

    float a = 0.5f;
    float b = 0.5f;
    //cout << "str:" << character.getCharacterStr() << endl;
    // if the charater is '1', its probalilty is redcued by its iou
    if ("1" == character.getCharacterStr()) {
      a = 0.3f; //0.2f;
      b = 0.7f; //0.8f;
    }
    float c = 0.1f;
    //float weighted_score = a * (float)score + b * w_ratio + c * h_ratio;
    float weighted_score = a * (float)score + b * w_ratio + c * h_ratio;
    SHOW_IMAGE(character.getCharacterMat(), 0);
    character.setCharacterScore((double)weighted_score);
    //cout << "weighted_score:" << character.getCharacterScore() << endl;
  }

  std::sort(inVec.begin(), inVec.end());

  std::vector<CCharacter>::iterator it = inVec.begin();
  for (; it != inVec.end(); ++it) {
    CCharacter charSrc = *it;
    // cout << "charScore:" << charSrc.getCharacterScore() << endl;
    Rect rectSrc = charSrc.getCharacterPos();
    std::vector<CCharacter>::iterator itc = it + 1;

    for (; itc != inVec.end();) {
      CCharacter charComp = *itc;
      Rect rectComp = charComp.getCharacterPos();
      float iou = computeIOU(rectSrc, rectComp);

      if (iou > overlap) {
        itc = inVec.erase(itc);
      }
      else {
        ++itc;
      }
    }
  }
}

int getNearestIndex(Point center, const vector<Point>& groundCenters) {
  int gc_size = int(groundCenters.size());
  int index = 0;
  int min_length = INT_MAX;
  for (int p = 0; p < gc_size; p++) {
    Point gc_point = groundCenters.at(p);
     int length_square = (gc_point.x - center.x) * (gc_point.x - center.x) +
       (gc_point.y - center.y) * (gc_point.y - center.y);
    //int length_square = abs(gc_point.x - center.x);
    if (length_square < min_length) {
      min_length = length_square;
      index = p;
    }
  }
  return index;
}

int CCharsSegment::charsSegmentUsingMSER(Mat input, vector<Mat>& resultVec, vector<Mat>& grayChars, Color color) {
  Mat grayImage;
  cvtColor(input, grayImage, CV_BGR2GRAY);
  std::vector<cv::Mat> bgrSplit;
  split(input, bgrSplit);

  //Mat grayChannel = clearLiuDingAndBorder(grayImage, color); //clearLiuDingAndBorder(grayImage, color);
  Mat grayChannel = grayImage;

  // Mat cropedGrayImage = grayImage;
  // generate all channgel images;
  vector<Mat> channelImages;
  bool useThreeChannel = false;
  channelImages.push_back(grayChannel);
  if (useThreeChannel) {
    for (int i = 0; i < 3; i++)
      channelImages.push_back(bgrSplit.at(i));
  }
  int csize = channelImages.size();

  //TODO three channels
  std::vector<std::vector<Point>> all_contours;
  std::vector<Rect> all_boxes;
  all_contours.reserve(32);
  all_boxes.reserve(32);

  const int imageArea = input.rows * input.cols;
  const int delta = 1;
  const int minArea = 30;
  const double maxAreaRatio = 0.2;

  int type = -1;
  if (Color::BLUE == color) type = 0;
  if (Color::YELLOW == color) type = 1;
  if (Color::WHITE == color) type = 1;
  if (Color::UNKNOWN == color) type = 0;

  for (int c_index = 0; c_index < csize; c_index++) {
    Mat cimage = channelImages.at(c_index);
    Mat testImage = cimage.clone();
    cvtColor(testImage, testImage, CV_GRAY2BGR);

    const float plateMaxSymbolCount = kPlateMaxSymbolCount;
    const int symbolIndex = kSymbolIndex;
    float segmentRatio = plateMaxSymbolCount - int(plateMaxSymbolCount);
    const int plateMaxCharCount = int(plateMaxSymbolCount);

    vector<vector<CCharacter>> charsVecVec;
    charsVecVec.resize(plateMaxCharCount);

    vector<Point> groundCenters;
    groundCenters.reserve(plateMaxCharCount);
    vector<Rect> groundRects;
    groundRects.reserve(plateMaxCharCount);

    // compute the ground char rect
    int avg_char_width = int(kPlateResizeWidth * (1.f / plateMaxSymbolCount));
    int avg_char_height = int(kPlateResizeHeight * 0.85f);

    int x_axis = 0;
    int y_axis = int((kPlateResizeHeight - avg_char_height) * 0.5f);
    for (int j = 0; j < plateMaxSymbolCount; j++) {
      int char_width = avg_char_width;
      if (j != symbolIndex) char_width = avg_char_width;
      else char_width = int(segmentRatio * avg_char_width);

      Rect avg_char_rect = Rect(x_axis, y_axis, char_width, avg_char_height);
      rectangle(testImage, avg_char_rect, Scalar(0, 0, 255));

      Point center = Point(x_axis + int(char_width * 0.5f), y_axis + int(avg_char_height * 0.5f));
      circle(testImage, center, 3, Scalar(0, 255, 0));
      x_axis += char_width;

      if (j != symbolIndex) {
        groundCenters.push_back(center);
        groundRects.push_back(avg_char_rect);
      }
    }
    SHOW_IMAGE(testImage, 0);

    Mat showImage = cimage.clone();
    cvtColor(showImage, showImage, CV_GRAY2BGR);
    Mat mdoImage = cimage.clone();
    string candidateLicense;

    Ptr<MSER2> mser;
    // use origin mser to detect as many as possible characters
    mser = MSER2::create(delta, minArea, int(maxAreaRatio * imageArea), false);
    mser->detectRegions(cimage, all_contours, all_boxes, type);

    std::vector<CCharacter> charVec;
    charVec.reserve(16);
    size_t size = all_contours.size();

    int char_index = 0;
    int char_size = 20;

    Mat showMSERImage = cimage.clone();
    cvtColor(showMSERImage, showMSERImage, CV_GRAY2BGR);
    // verify char size and output to rects;
    for (size_t index = 0; index < size; index++) {
      Rect rect = all_boxes[index];
      vector<Point> &contour = all_contours[index];
      rectangle(showMSERImage, rect, Scalar(0,0,255));

      // find character
      if (verifyCharRectSizes(rect)) {
        Mat mserMat = adaptive_image_from_points(contour, rect, Size(char_size, char_size));
        Mat mserInput = preprocessCharMat(mserMat, char_size);

        Rect charRect = rect;
        Point center(charRect.tl().x + charRect.width / 2, charRect.tl().y + charRect.height / 2);
        Mat tmpMat;
        double ostu_level = cv::threshold(cimage(charRect), tmpMat, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
        Mat grayCharMat = cimage(charRect);
        Mat ostuMat;
        switch (color) {
          case BLUE:   threshold(grayCharMat, ostuMat, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU); break;
          case YELLOW:   threshold(grayCharMat, ostuMat, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU); break;
          case WHITE:  threshold(grayCharMat, ostuMat, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV); break;
          default: threshold(grayCharMat, ostuMat, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY); break;
        }
        Mat ostuInput = preprocessChar(ostuMat);
        // use judegMDOratio2 function to
        // remove the small lines in character like "zh-cuan"
        if (judegMDOratio2(cimage, rect, contour, mdoImage, 1.2f, true)) {
          CCharacter charCandidate;
          //cout << contour.size() << endl;
          charCandidate.setCharacterPos(charRect);
          charCandidate.setCharacterMat(ostuInput);   //charInput or ostuInput
          charCandidate.setOstuLevel(ostu_level);
          charCandidate.setCenterPoint(center);
          int pos = getNearestIndex(center, groundCenters);
          charsVecVec.at(pos).push_back(charCandidate);
          charCandidate.setIndex(pos);
          charCandidate.setIsChinese(false);
          charVec.push_back(charCandidate);
        }
      }
      else {
        SHOW_IMAGE(showMSERImage(rect), 0);
      }
    }
    SHOW_IMAGE(showMSERImage, 0);
    SHOW_IMAGE(mdoImage, 0);

    // classify all the images;
    CharsIdentify::instance()->classify(charVec);
    Rect maxrect = groundRects.at(0);

    // NMS to the seven groud truth rect 
    bool useGround = true;
    if (useGround) {
      for (auto charCandidate : charVec) {
        int pos = charCandidate.getIndex();
        charsVecVec.at(pos).push_back(charCandidate);
      }
      charVec.clear();
      for (size_t c = 0; c < charsVecVec.size(); c++) {
        Mat testImage_2 = cimage.clone();
        cvtColor(testImage_2, testImage_2, CV_GRAY2BGR);
        vector<CCharacter>& charPosVec = charsVecVec.at(c);
        for (auto character : charPosVec) {
          rectangle(testImage_2, character.getCharacterPos(), Scalar(0, 255, 0));
        }
        SHOW_IMAGE(testImage_2, 0);

        double overlapThresh = 0.;
        NMStoCharacterByRatio(charPosVec, overlapThresh, groundRects.at(c));
        charPosVec.shrink_to_fit();

        Mat testImage_3 = cimage.clone();
        cvtColor(testImage_3, testImage_3, CV_GRAY2BGR);
        for (auto character : charPosVec) {
          rectangle(testImage_3, character.getCharacterPos(), Scalar(0, 255, 0));
        }

        // only the last group will contain more than one candidate character
        if (charsVecVec.size() - 1 == c) {
          for (auto charPos : charPosVec)
            charVec.push_back(charPos);
        }
        else {
          if (charPosVec.size() != 0) {
            CCharacter& inputChar = charPosVec.at(0);
            charVec.push_back(inputChar);
            Mat charMat = inputChar.getCharacterMat();
            SHOW_IMAGE(charMat, 0);
          }
        }
        for (auto charPos : charPosVec) {
          Rect r = charPos.getCharacterPos();
          if (r.area() > maxrect.area())
            maxrect = r;
        }
        SHOW_IMAGE(testImage_3, 0);
      }
    }
    else {
      NMStoCharacterByRatio(charVec, 0.2f, maxrect);
    }

    if (charVec.size() < kCharsCountInOnePlate) return 0x03;
    std::sort(charVec.begin(), charVec.end(),[](const CCharacter& r1, const CCharacter& r2) { return r1.getCharacterPos().x < r2.getCharacterPos().x; });

    string predictLicense = "";
    vector<Rect> sortedRect;
    for (auto charCandidate : charVec) {
      sortedRect.push_back(charCandidate.getCharacterPos());
      predictLicense.append(charCandidate.getCharacterStr());
    }
    std::sort(sortedRect.begin(), sortedRect.end(),
      [](const Rect& r1, const Rect& r2) { return r1.x < r2.x; });
    cout << "predictLicense: " << predictLicense << endl;

    // find chinese rect
    size_t specIndex = 0;
    specIndex = GetSpecificRect(sortedRect);
    SHOW_IMAGE(showImage(sortedRect[specIndex]), 0);

    Rect chineseRect;
    if (specIndex < sortedRect.size())
      chineseRect = GetChineseRect(sortedRect[specIndex]);
    else
      return 0x04;

    vector<Rect> newSortedRect;
    newSortedRect.push_back(chineseRect);
    if (newSortedRect.size() == 0) return 0x05;

    SHOW_IMAGE(showImage(chineseRect), 0);
    RebuildRect(sortedRect, newSortedRect, specIndex);

    Mat theImage = channelImages.at(c_index);
    for (size_t i = 0; i < newSortedRect.size(); i++) {
      Rect mr = newSortedRect[i];
      //mr = rectEnlarge(newSortedRect[i], cimage.cols, cimage.rows);
      Mat auxRoi(theImage, mr);
      Mat newRoi;
      if (i == 0) {
        //Rect large_mr = rectEnlarge(mr, theImage.cols, theImage.rows);
        Rect large_mr = mr;
        Mat grayChar(theImage, large_mr);
        Mat grayChinese;
        grayChinese.create(kGrayCharHeight, kGrayCharWidth, CV_8UC1);
        resize(grayChar, grayChinese, grayChinese.size(), 0, 0, INTER_LINEAR);

        Mat newChineseRoi;
        if (1) {
          float slideLengthRatio = 0.1f;
          if (!slideChineseGrayWindow(theImage, large_mr, newChineseRoi, color, slideLengthRatio))
            judgeChineseGray(grayChinese, newChineseRoi, color);
        }
        grayChars.push_back(newChineseRoi);
      }
      else {
        switch (color) {
          case BLUE:   threshold(auxRoi, newRoi, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU); break;
          case YELLOW:   threshold(auxRoi, newRoi, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU); break;
          case WHITE:  threshold(auxRoi, newRoi, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV); break;
          default: threshold(auxRoi, newRoi, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY); break;
        }
        newRoi = preprocessChar(newRoi);
        Rect fit_mr = rectFit(mr, cimage.cols, cimage.rows);
        Mat grayChar(cimage, fit_mr);
        grayChars.push_back(grayChar);
      }

      rectangle(showImage, mr, Scalar(0, 0, 255), 1);
      resultVec.push_back(newRoi);
    }
    SHOW_IMAGE(showImage, 0);
  }

  return 0;
}


int CCharsSegment::charsSegmentUsingOSTU(Mat input, vector<Mat>& resultVec, vector<Mat>& grayChars, Color color) {
  if (!input.data) return 0x01;

  Color plateType = color;
  Mat input_grey;
  cvtColor(input, input_grey, CV_BGR2GRAY);

  Mat img_threshold;
  img_threshold = input_grey.clone();
  spatial_ostu(img_threshold, 8, 2, plateType);

  // remove liuding and hor lines, also judge weather is plate use jump count
  if (!clearLiuDing(img_threshold)) return 0x02;

  Mat img_contours;
  img_threshold.copyTo(img_contours);

  vector<vector<Point> > contours;
  findContours(img_contours,
               contours,               // a vector of contours
               CV_RETR_EXTERNAL,       // retrieve the external contours
               CV_CHAIN_APPROX_NONE);  // all pixels of each contours

  vector<vector<Point> >::iterator itc = contours.begin();
  vector<Rect> vecRect;
  while (itc != contours.end()) {
    Rect mr = boundingRect(Mat(*itc));
    Mat auxRoi(img_threshold, mr);
    if (verifyCharSizes(auxRoi))
      vecRect.push_back(mr);
    ++itc;
  }

  if (vecRect.size() == 0) return 0x03;

  vector<Rect> sortedRect(vecRect);
  std::sort(sortedRect.begin(), sortedRect.end(),
            [](const Rect& r1, const Rect& r2) { return r1.x < r2.x; });

  size_t specIndex = 0;
  specIndex = GetSpecificRect(sortedRect);

  Rect chineseRect;
  if (specIndex < sortedRect.size())
    chineseRect = GetChineseRect(sortedRect[specIndex]);
  else
    return 0x04;

  if (0) {
    rectangle(img_threshold, chineseRect, Scalar(255));
    imshow("plate", img_threshold);
    waitKey(0);
    destroyWindow("plate");
  }

  vector<Rect> newSortedRect;
  newSortedRect.push_back(chineseRect);
  RebuildRect(sortedRect, newSortedRect, specIndex);

  if (newSortedRect.size() == 0) return 0x05;

  bool useSlideWindow = true;
  bool useAdapThreshold = true;
  //bool useAdapThreshold = CParams::instance()->getParam1b();

  for (size_t i = 0; i < newSortedRect.size(); i++) {
    Rect mr = newSortedRect[i];
    Mat auxRoi(input_grey, mr);
    Mat newRoi;

    if (i == 0) {
      // genenrate gray chinese char
      Rect large_mr = rectEnlarge(mr, input_grey.cols, input_grey.rows);
      Mat grayChar(input_grey, large_mr);
      Mat grayChinese;
      grayChinese.create(kGrayCharHeight, kGrayCharWidth, CV_8UC1);
      resize(grayChar, grayChinese, grayChinese.size(), 0, 0, INTER_LINEAR);

      Mat newChineseRoi;
      if (useSlideWindow) {
        float slideLengthRatio = 0.1f;
        if (!slideChineseGrayWindow(input_grey, large_mr, newChineseRoi, plateType, slideLengthRatio))
          judgeChineseGray(grayChinese, newChineseRoi, plateType);
      }
      else {
        judgeChinese(auxRoi, newRoi, plateType);
      }
      grayChars.push_back(newChineseRoi);
    }
    else {
      switch (plateType) {
        case BLUE:   threshold(auxRoi, newRoi, 0, 255, CV_THRESH_BINARY + CV_THRESH_OTSU); break;
        case YELLOW:   threshold(auxRoi, newRoi, 0, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU); break;
        case WHITE:  threshold(auxRoi, newRoi, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV); break;
        default: threshold(auxRoi, newRoi, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY); break;
      }
      newRoi = preprocessChar(newRoi);

      // genenrate gray chinese char
      Rect fit_mr = rectFit(mr, input_grey.cols, input_grey.rows);
      Mat grayChar(input_grey, fit_mr);
      grayChars.push_back(grayChar);
    }
    resultVec.push_back(newRoi);
  }
  return 0;
}


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

    // use prior knowledage to find the specific character
    // position in 1/7 and 2/7
    if ((mr.width > maxWidth * 0.6 || mr.height > maxHeight * 0.6) &&
      (midx < int(m_theMatWidth / kPlateMaxSymbolCount) * kSymbolIndex &&
      midx > int(m_theMatWidth / kPlateMaxSymbolCount) * (kSymbolIndex - 1))) {
      specIndex = i;
    }
  }

  return specIndex;
}

int CCharsSegment::RebuildRect(const vector<Rect>& vecRect,
                               vector<Rect>& outRect, int specIndex) {
  int count = 6;
  for (size_t i = specIndex; i < vecRect.size() && count; ++i, --count) {
    outRect.push_back(vecRect[i]);
  }

  return 0;
}

}
