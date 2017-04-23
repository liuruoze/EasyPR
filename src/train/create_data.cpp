#include "easypr/train/create_data.h"

namespace easypr {

  int getBoderColor(Mat img) {
    assert(img.channels() == 1);
    assert(img.type() == CV_8UC1);
    int w = img.cols;
    int h = img.rows;

    float sum = 0;
    for (int i = 0; i < h; ++i) {
      sum += img.at<unsigned char>(i, 0);
      sum += img.at<unsigned char>(i, w-1);
    }
    for (int j = 0; j < w; ++j) {
      sum += img.at<unsigned char>(0, j);
      sum += img.at<unsigned char>(h-1, j);
    }

    float avg = sum / float(w + w + h + h);
    return int(avg);
  }

  // shift an image
  Mat translateImg(Mat img, int offsetx, int offsety, int bk){
    Mat dst;
    //cout << test << endl;
    Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
    //cout << trans_mat << endl;
    warpAffine(img, dst, trans_mat, img.size(), 1, 0, Scalar(bk));
    return dst;
  }

  // rotate an image
  Mat rotateImg(Mat source, float angle, int bk){
    Point2f src_center(source.cols / 2.0F, source.rows / 2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size(), 1, 0, Scalar(bk));
    return dst;
  }

  // crop the image
  Mat cropImg(Mat src, int x, int y, int shift, int bk){
    int width = src.cols;
    int height = src.rows;

    int crop_width = width - shift;
    int crop_height = height - shift;

    int x_shift = shift;
    int y_shift = shift;

    x = x < x_shift ? x : x_shift;
    y = y < y_shift ? y : y_shift;

    Rect rect = Rect(x, y, crop_width, crop_height);

    Mat dst = src(rect);
    resize(dst, dst, Size(width, height));
    return dst;
  }

  Mat generateSyntheticImage(const Mat& image, int use_swap) {
    int rd = rand();
    int bkColor = getBoderColor(image);
    Mat result = image.clone();
    if (0 && (rd >> 6 & 1)) {
      int shift = 2;
      int ran_x = rand() % shift;
      int ran_y = rand() % shift;
      result = cropImg(result, ran_x, ran_y, shift, bkColor);
    }
    if (0 && (rd >> 4 & 1)) {
      int ran_x = rand() % 2 - 1;
      int ran_y = rand() % 2 - 1;
      result = translateImg(result, ran_x, ran_y, bkColor);
    }
    if (1 && (rd >> 2 & 1)) {
      float angle = float(rand() % 100) * 0.1f - 5.f;
      result = rotateImg(result, angle, bkColor);
    }

    return result;
  }
}

