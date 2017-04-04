#ifndef EASYPR_CREATE_DATA_H_
#define EASYPR_CREATE_DATA_H_

#include "opencv2/opencv.hpp"
#include "easypr/config.h"

using namespace cv;
using namespace std;

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  // shift an image
  Mat translateImg(Mat img, int offsetx, int offsety);
  // rotate an image
  Mat rotateImg(Mat source, float angle);

  // crop the image
  Mat cropImg(Mat src, int x, int y, int shift);

  Mat generateSyntheticImage(const Mat& image, int use_swap = 1);

} /*! \namespace easypr*/

#endif  // EASYPR_CREATE_DATA_H_