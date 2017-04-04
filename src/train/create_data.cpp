#include "easypr/train/create_data.h"

namespace easypr {

  // shift an image
  Mat translateImg(Mat img, int offsetx, int offsety){
    Mat dst;
    Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
    warpAffine(img, dst, trans_mat, img.size(), 1, 0, Scalar(255));
    return dst;
  }

  // rotate an image
  Mat rotateImg(Mat source, float angle){
    Point2f src_center(source.cols / 2.0F, source.rows / 2.0F);
    Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
    Mat dst;
    warpAffine(source, dst, rot_mat, source.size(), 1, 0, Scalar(255));
    return dst;
  }

  // crop the image
  Mat cropImg(Mat src, int x, int y, int shift){
    int width = kGrayCharWidth;
    int height = kGrayCharHeight;

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
    Mat result = image.clone();

    if (rd >> 0 & 1) {
      int shift = 4;
      int ran_x = rand() % shift;
      int ran_y = rand() % shift;
      result = cropImg(result, ran_x, ran_y, shift);
    }
    if (rd >> 1 & 1) {

    }
    if (rd >> 2 & 1) {

    }
    if (rd >> 3 & 1) {
      int ran_x = rand() % 6 - 3;
      int ran_y = rand() % 6 - 3;
      result = translateImg(result, ran_x, ran_y);
    }
    if (rd >> 4 & 1) {
      float angle = float(rand() % 20 - 10);
      result = rotateImg(result, angle);
    }

    return result;
  }
}

