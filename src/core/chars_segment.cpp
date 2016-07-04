#include "easypr/core/chars_segment.h"
#include "easypr/core/chars_identify.h"
#include "easypr/core/core_func.h"
#include "easypr/core/params.h"
#include "easypr/config.h"

using namespace std;

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

  std::cout << "valOstu: " << valOstu << std::endl;
  std::cout << "valAdap: " << valAdap << std::endl;

  if (valOstu >= valAdap) {
    out = roiOstu;
  }
  else {
    out = roiAdap;
  }

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


  //if (BLUE == plateType) {
  //  // cout << "BLUE" << endl;
  //  img_threshold = input_grey.clone();

  //  int w = input_grey.cols;
  //  int h = input_grey.rows;
  //  Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
  //  int threadHoldV = ThresholdOtsu(tmp);
  //  threshold(input_grey, img_threshold, threadHoldV, 255, CV_THRESH_BINARY);

  //} else if (YELLOW == plateType) {
  //  // cout << "YELLOW" << endl;
  //  img_threshold = input_grey.clone();
  //  int w = input_grey.cols;
  //  int h = input_grey.rows;
  //  Mat tmp = input_grey(Rect_<double>(w * 0.1, h * 0.1, w * 0.8, h * 0.8));
  //  int threadHoldV = ThresholdOtsu(tmp);
  //  // utils::imwrite("resources/image/tmp/inputgray2.jpg", input_grey);

  //  threshold(input_grey, img_threshold, threadHoldV, 255,
  //            CV_THRESH_BINARY_INV);

  //} else if (WHITE == plateType) {
  //  // cout << "WHITE" << endl;

  //  threshold(input_grey, img_threshold, 10, 255,
  //            CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
  //} else {
  //  // cout << "UNKNOWN" << endl;
  //  threshold(input_grey, img_threshold, 10, 255,
  //            CV_THRESH_OTSU + CV_THRESH_BINARY);
  //}

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
  //clearLiuDing(img_threshold);


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

    // use known knowledage to find the specific character
    // position in 1/7 and 2/7
    if ((mr.width > maxWidth * 0.8 || mr.height > maxHeight * 0.8) &&
        (midx < int(m_theMatWidth / 7) * 2 &&
         midx > int(m_theMatWidth / 7) * 1)) {
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
