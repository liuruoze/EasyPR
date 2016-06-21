#include "lbp.hpp"
#include "helper.hpp"

using namespace cv;

//------------------------------------------------------------------------------
// cv::olbp
//------------------------------------------------------------------------------
namespace libfacerec {

  template <typename _Tp> static
    void olbp_(InputArray _src, OutputArray _dst) {
      // get matrices
      Mat src = _src.getMat();
      // allocate memory for result
      _dst.create(src.rows - 2, src.cols - 2, CV_8UC1);
      Mat dst = _dst.getMat();
      // zero the result matrix
      dst.setTo(0);
      // calculate patterns
      for (int i = 1; i<src.rows - 1; i++) {
        for (int j = 1; j<src.cols - 1; j++) {
          _Tp center = src.at<_Tp>(i, j);
          unsigned char code = 0;
          code |= (src.at<_Tp>(i - 1, j - 1) >= center) << 7;
          code |= (src.at<_Tp>(i - 1, j) >= center) << 6;
          code |= (src.at<_Tp>(i - 1, j + 1) >= center) << 5;
          code |= (src.at<_Tp>(i, j + 1) >= center) << 4;
          code |= (src.at<_Tp>(i + 1, j + 1) >= center) << 3;
          code |= (src.at<_Tp>(i + 1, j) >= center) << 2;
          code |= (src.at<_Tp>(i + 1, j - 1) >= center) << 1;
          code |= (src.at<_Tp>(i, j - 1) >= center) << 0;
          dst.at<unsigned char>(i - 1, j - 1) = code;
        }
      }
    }

}

void libfacerec::olbp(InputArray src, OutputArray dst) {
  switch (src.getMat().type()) {
  case CV_8SC1:   olbp_<char>(src, dst); break;
  case CV_8UC1:   olbp_<unsigned char>(src, dst); break;
  case CV_16SC1:  olbp_<short>(src, dst); break;
  case CV_16UC1:  olbp_<unsigned short>(src, dst); break;
  case CV_32SC1:  olbp_<int>(src, dst); break;
  case CV_32FC1:  olbp_<float>(src, dst); break;
  case CV_64FC1:  olbp_<double>(src, dst); break;
  default: break;
  }
}

//------------------------------------------------------------------------------
// cv::varlbp
//------------------------------------------------------------------------------
namespace libfacerec {

  template <typename _Tp> static
    inline void varlbp_(InputArray _src, OutputArray _dst, int radius, int neighbors) {
      //get matrices
      Mat src = _src.getMat();
      // allocate memory for result
      _dst.create(src.rows - 2 * radius, src.cols - 2 * radius, CV_32FC1);
      Mat dst = _dst.getMat();
      // set initial values to zero
      dst.setTo(0.0);
      // allocate some memory for temporary on-line variance calculations
      Mat _mean = Mat::zeros(src.rows, src.cols, CV_32FC1);
      Mat _delta = Mat::zeros(src.rows, src.cols, CV_32FC1);
      Mat _m2 = Mat::zeros(src.rows, src.cols, CV_32FC1);
      for (int n = 0; n<neighbors; n++) {
        // sample points
        float x = static_cast<float>(radius)* cos(2.0f * (float)CV_PI*n / static_cast<float>(neighbors));
        float y = static_cast<float>(radius)* -sin(2.0f * (float)CV_PI*n / static_cast<float>(neighbors));
        // relative indices
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));
        // fractional part
        float ty = y - fy;
        float tx = x - fx;
        // set interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 = tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 = tx  *      ty;
        // iterate through your data
        for (int i = radius; i < src.rows - radius; i++) {
          for (int j = radius; j < src.cols - radius; j++) {
            float t = static_cast<float>(w1*src.at<_Tp>(i + fy, j + fx) + w2*src.at<_Tp>(i + fy, j + cx) + w3*src.at<_Tp>(i + cy, j + fx) + w4*src.at<_Tp>(i + cy, j + cx));
            _delta.at<float>(i, j) = t - _mean.at<float>(i, j);
            _mean.at<float>(i, j) = (_mean.at<float>(i, j) + (_delta.at<float>(i, j) / (1.0f * (n + 1)))); // i am a bit paranoid
            _m2.at<float>(i, j) = _m2.at<float>(i, j) + _delta.at<float>(i, j) * (t - _mean.at<float>(i, j));
          }
        }
      }
      // calculate result
      for (int i = radius; i < src.rows - radius; i++) {
        for (int j = radius; j < src.cols - radius; j++) {
          dst.at<float>(i - radius, j - radius) = _m2.at<float>(i, j) / (1.0f * (neighbors - 1));
        }
      }
    }

}

void libfacerec::varlbp(InputArray src, OutputArray dst, int radius, int neighbors) {
  switch (src.getMat().type()) {
  case CV_8SC1:   varlbp_<char>(src, dst, radius, neighbors); break;
  case CV_8UC1:   varlbp_<unsigned char>(src, dst, radius, neighbors); break;
  case CV_16SC1:  varlbp_<short>(src, dst, radius, neighbors); break;
  case CV_16UC1:  varlbp_<unsigned short>(src, dst, radius, neighbors); break;
  case CV_32SC1:  varlbp_<int>(src, dst, radius, neighbors); break;
  case CV_32FC1:  varlbp_<float>(src, dst, radius, neighbors); break;
  case CV_64FC1:  varlbp_<double>(src, dst, radius, neighbors); break;
  default: break;
  }
}

//------------------------------------------------------------------------------
// cv::elbp
//------------------------------------------------------------------------------
namespace libfacerec {
  template <typename _Tp> static
    inline void elbp_(InputArray _src, OutputArray _dst, int radius, int neighbors) {
      //get matrices
      Mat src = _src.getMat();
      // allocate memory for result
      _dst.create(src.rows - 2 * radius, src.cols - 2 * radius, CV_32SC1);
      Mat dst = _dst.getMat();
      // zero
      dst.setTo(0);
      for (int n = 0; n<neighbors; n++) {
        // sample points
        float x = static_cast<float>(-radius) * sin(2.0f*(float)CV_PI*n / static_cast<float>(neighbors));
        float y = static_cast<float>(radius)* cos(2.0f*(float)CV_PI*n / static_cast<float>(neighbors));
        // relative indices
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));
        // fractional part
        float ty = y - fy;
        float tx = x - fx;
        // set interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 = tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 = tx  *      ty;
        // iterate through your data
        for (int i = radius; i < src.rows - radius; i++) {
          for (int j = radius; j < src.cols - radius; j++) {
            // calculate interpolated value
            float t = static_cast<float>(w1*src.at<_Tp>(i + fy, j + fx) + w2*src.at<_Tp>(i + fy, j + cx) + w3*src.at<_Tp>(i + cy, j + fx) + w4*src.at<_Tp>(i + cy, j + cx));
            // floating point precision, so check some machine-dependent epsilon
            dst.at<int>(i - radius, j - radius) += ((t > src.at<_Tp>(i, j)) || (std::abs(t - src.at<_Tp>(i, j)) < std::numeric_limits<float>::epsilon())) << n;
          }
        }
      }
    }
}

namespace libfacerec {
  template <typename _Tp> static
    inline void elbp0(const Mat& src, Mat& dst, int radius, int neighbors) {
      neighbors = max(min(neighbors, 31), 1); // set bounds...
      // Note: alternatively you can switch to the new OpenCV Mat_
      // type system to define an unsigned int matrix... I am probably
      // mistaken here, but I didn't see an unsigned int representation
      // in OpenCV's classic typesystem...
      dst = Mat::zeros(src.rows - 2 * radius, src.cols - 2 * radius, CV_32SC1);
      for (int n = 0; n < neighbors; n++) {
        // sample points
        float x = static_cast<float>(radius)* cos(2.0f * (float)CV_PI * n / static_cast<float>(neighbors));
        float y = static_cast<float>(radius)* -sin(2.0f * (float)CV_PI * n / static_cast<float>(neighbors));
        // relative indices
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));
        // fractional part
        float ty = y - fy;
        float tx = x - fx;
        // set interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 = tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 = tx  *      ty;
        // iterate through your data
        for (int i = radius; i < src.rows - radius; i++) {
          for (int j = radius; j < src.cols - radius; j++) {
            float t = static_cast<float>(w1*src.at<_Tp>(i + fy, j + fx) + w2*src.at<_Tp>(i + fy, j + cx) + w3*src.at<_Tp>(i + cy, j + fx) + w4*src.at<_Tp>(i + cy, j + cx));
            // we are dealing with floating point precision, so add some little tolerance
            dst.at<unsigned int>(i - radius, j - radius) += ((t > src.at<_Tp>(i, j)) && (abs(t - src.at<_Tp>(i, j)) > std::numeric_limits<float>::epsilon())) << n;
          }
        }
      }
    }
}

void libfacerec::elbp0(const Mat& src, Mat& dst, int radius, int neighbors) {
  switch (src.type()) {
  case CV_8SC1:   elbp0<char>(src, dst, radius, neighbors); break;
  case CV_8UC1:   elbp0<unsigned char>(src, dst, radius, neighbors); break;
  case CV_16SC1:  elbp0<short>(src, dst, radius, neighbors); break;
  case CV_16UC1:  elbp0<unsigned short>(src, dst, radius, neighbors); break;
  case CV_32SC1:  elbp0<int>(src, dst, radius, neighbors); break;
  case CV_32FC1:  elbp0<float>(src, dst, radius, neighbors); break;
  case CV_64FC1:  elbp0<double>(src, dst, radius, neighbors); break;
  default: break;
  }
}


void libfacerec::elbp(InputArray src, OutputArray dst, int radius, int neighbors) {
  switch (src.type()) {
  case CV_8SC1:   elbp_<char>(src, dst, radius, neighbors); break;
  case CV_8UC1:   elbp_<unsigned char>(src, dst, radius, neighbors); break;
  case CV_16SC1:  elbp_<short>(src, dst, radius, neighbors); break;
  case CV_16UC1:  elbp_<unsigned short>(src, dst, radius, neighbors); break;
  case CV_32SC1:  elbp_<int>(src, dst, radius, neighbors); break;
  case CV_32FC1:  elbp_<float>(src, dst, radius, neighbors); break;
  case CV_64FC1:  elbp_<double>(src, dst, radius, neighbors); break;
  default: break;
  }
}

Mat libfacerec::spatial_histogram(InputArray _src, int numPatterns, int grid_x, int grid_y, bool normed) {
  Mat src = _src.getMat();
  // calculate LBP patch size
  int width = src.cols / grid_x;
  int height = src.rows / grid_y;
  // allocate memory for the spatial histogram
  Mat result = Mat::zeros(grid_x * grid_y, numPatterns, CV_32FC1);
  // return matrix with zeros if no data was given
  if (src.empty())
    return result.reshape(1, 1);
  // initial result_row
  int resultRowIdx = 0;
  // iterate through grid
  for (int i = 0; i < grid_y; i++) {
    for (int j = 0; j < grid_x; j++) {
      Mat src_cell = Mat(src, Range(i*height, (i + 1)*height), Range(j*width, (j + 1)*width));
      Mat cell_hist = histc(src_cell, 0, (numPatterns - 1), true);
      // copy to the result matrix
      Mat result_row = result.row(resultRowIdx);
      cell_hist.reshape(1, 1).convertTo(result_row, CV_32FC1);
      // increase row count in result matrix
      resultRowIdx++;
    }
  }
  // return result as reshaped feature vector
  return result.reshape(1, 1);
}

//------------------------------------------------------------------------------
// cv::elbp, cv::olbp, cv::varlbp wrapper
//------------------------------------------------------------------------------
Mat libfacerec::olbp(InputArray src) {
  Mat dst;
  olbp(src, dst);
  return dst;
}

Mat libfacerec::elbp(InputArray src, int radius, int neighbors) {
  Mat dst;
  elbp(src, dst, radius, neighbors);
  return dst;
}

Mat libfacerec::varlbp(InputArray src, int radius, int neighbors) {
  Mat dst;
  varlbp(src, dst, radius, neighbors);
  return dst;
}