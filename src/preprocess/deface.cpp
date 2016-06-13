#include "easypr/preprocess/deface.h"

namespace easypr {

namespace preprocess {

cv::Mat detectAndMaskFace(cv::Mat& img, cv::CascadeClassifier& cascade,
                          double scale) {
  std::vector<cv::Rect> faces;
  cv::Mat gray,
      smallImg(cvRound(img.rows / scale), cvRound(img.cols / scale), CV_8UC1);
  cvtColor(img, gray, cv::COLOR_BGR2GRAY);
  resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
  equalizeHist(smallImg, smallImg);

  cascade.detectMultiScale(smallImg, faces, 1.1, 2,
                           0
                               //|CASCADE_FIND_BIGGEST_OBJECT
                               //|CASCADE_DO_ROUGH_SEARCH
                               | cv::CASCADE_SCALE_IMAGE,
                           cv::Size(30, 30));
  for (auto r = faces.begin(); r != faces.end(); r++) {
    cv::Rect facerect = *r;
    cv::Mat roi =
        img(cv::Rect_<double>(facerect.x * scale, facerect.y * scale,
                              facerect.width * scale, facerect.height * scale));
    int W = 18;
    int H = 18;
    for (int i = W; i < roi.cols; i += W) {
      for (int j = H; j < roi.rows; j += H) {
        uchar s = roi.at<uchar>(j - H / 2, (i - W / 2) * 3);
        uchar s1 = roi.at<uchar>(j - H / 2, (i - W / 2) * 3 + 1);
        uchar s2 = roi.at<uchar>(j - H / 2, (i - W / 2) * 3 + 2);
        for (int ii = i - W; ii <= i; ii++) {
          for (int jj = j - H; jj <= j; jj++) {
            roi.at<uchar>(jj, ii * 3 + 0) = s;
            roi.at<uchar>(jj, ii * 3 + 1) = s1;
            roi.at<uchar>(jj, ii * 3 + 2) = s2;
          }
        }
      }
    }
  }

  return img;
}

int deface() {
  cv::CascadeClassifier cascade;
  std::string cascadeName =
      "resources/model/haarcascade_frontalface_default.xml";

  if (!cascade.load(cascadeName)) {
    std::cerr << "ERROR: Could not load classifier cascade" << std::endl;
    return -1;
  }

  return 0;
}

}  // namespace preprocess

}  // namespace easypr
