/*
 * Copyright (c) 2012. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */
#include <opencv2/opencv.hpp>
#include "helper.hpp"

using namespace cv;

//------------------------------------------------------------------------------
// libfacerec::isSymmetric
//------------------------------------------------------------------------------
namespace libfacerec {

template<typename _Tp> static bool
isSymmetric_(InputArray src) {
    Mat _src = src.getMat();
    if(_src.cols != _src.rows)
        return false;
    for (int i = 0; i < _src.rows; i++) {
        for (int j = 0; j < _src.cols; j++) {
            _Tp a = _src.at<_Tp> (i, j);
            _Tp b = _src.at<_Tp> (j, i);
            if (a != b) {
                return false;
            }
        }
    }
    return true;
}

template<typename _Tp> static bool
isSymmetric_(InputArray src, double eps) {
    Mat _src = src.getMat();
    if(_src.cols != _src.rows)
        return false;
    for (int i = 0; i < _src.rows; i++) {
        for (int j = 0; j < _src.cols; j++) {
            _Tp a = _src.at<_Tp> (i, j);
            _Tp b = _src.at<_Tp> (j, i);
            if (std::abs(a - b) > eps) {
                return false;
            }
        }
    }
    return true;
}

}

bool libfacerec::isSymmetric(InputArray src, double eps) {
    Mat m = src.getMat();
    switch (m.type()) {
    case CV_8SC1: return isSymmetric_<char>(m); break;
    case CV_8UC1:
        return isSymmetric_<unsigned char>(m); break;
    case CV_16SC1:
        return isSymmetric_<short>(m); break;
    case CV_16UC1:
        return isSymmetric_<unsigned short>(m); break;
    case CV_32SC1:
        return isSymmetric_<int>(m); break;
    case CV_32FC1:
        return isSymmetric_<float>(m, eps); break;
    case CV_64FC1:
        return isSymmetric_<double>(m, eps); break;
    default:
        break;
    }
    return false;
}

//------------------------------------------------------------------------------
// libfacerec::argsort
//------------------------------------------------------------------------------
Mat libfacerec::argsort(InputArray _src, bool ascending) {
    Mat src = _src.getMat();
    if (src.rows != 1 && src.cols != 1) {
        CV_Error(CV_StsBadArg, "cv::argsort only sorts 1D matrices.");
    }
    int flags = CV_SORT_EVERY_ROW+(ascending ? CV_SORT_ASCENDING : CV_SORT_DESCENDING);
    Mat sorted_indices;
    cv::sortIdx(src.reshape(1,1),sorted_indices,flags);
    return sorted_indices;
}
//------------------------------------------------------------------------------
// libfacerec::histc
//------------------------------------------------------------------------------
namespace libfacerec {

static Mat
histc_(const Mat& src, int minVal=0, int maxVal=255, bool normed=false) {
    Mat result;
    // Establish the number of bins.
    int histSize = maxVal-minVal+1;
    // Set the ranges.
    float range[] = {static_cast<float>(minVal), static_cast<float>(maxVal + 1)} ;
    const float* histRange = { range };
    // calc histogram
    calcHist(&src, 1, 0, Mat(), result, 1, &histSize, &histRange, true, false);
    // normalize
    if(normed) {
        result /= src.total();
    }
    return result.reshape(1,1);
}

}

Mat libfacerec::histc(InputArray _src, int minVal, int maxVal, bool normed) {
    Mat src = _src.getMat();
    switch (src.type()) {
    case CV_8SC1:
        return histc_(Mat_<float>(src), minVal, maxVal, normed);
        break;
    case CV_8UC1:
        return histc_(src, minVal, maxVal, normed);
        break;
    case CV_16SC1:
        return histc_(Mat_<float>(src), minVal, maxVal, normed);
        break;
    case CV_16UC1:
        return histc_(src, minVal, maxVal, normed);
        break;
    case CV_32SC1:
        return histc_(Mat_<float>(src), minVal, maxVal, normed);
        break;
    case CV_32FC1:
        return histc_(src, minVal, maxVal, normed);
        break;
    default:
        CV_Error(CV_StsUnmatchedFormats, "This type is not implemented yet."); break;
    }
    return Mat();
}

//------------------------------------------------------------------------------
// libfacerec::sortMatrixColumnsByIndices
//------------------------------------------------------------------------------

void libfacerec::sortMatrixColumnsByIndices(InputArray _src, InputArray _indices, OutputArray _dst) {
    if(_indices.getMat().type() != CV_32SC1) {
    	string error_message = format("cv::sortRowsByIndices only works on integer indices! Expected: %d. Given: %d.", CV_32SC1, _indices.getMat().type());
    	CV_Error(CV_StsBadArg, error_message);
    }
    Mat src = _src.getMat();
    vector<int> indices = _indices.getMat();
    _dst.create(src.rows, src.cols, src.type());
    Mat dst = _dst.getMat();
    for(size_t idx = 0; idx < indices.size(); idx++) {
        Mat originalCol = src.col(indices[idx]);
        Mat sortedCol = dst.col(idx);
        originalCol.copyTo(sortedCol);
    }
}

Mat libfacerec::sortMatrixColumnsByIndices(InputArray src, InputArray indices) {
    Mat dst;
    sortMatrixColumnsByIndices(src, indices, dst);
    return dst;
}

//------------------------------------------------------------------------------
// libfacerec::sortMatrixRowsByIndices
//------------------------------------------------------------------------------
void libfacerec::sortMatrixRowsByIndices(InputArray _src, InputArray _indices, OutputArray _dst) {
    if(_indices.getMat().type() != CV_32SC1) {
    	string error_message = format("cv::sortRowsByIndices only works on integer indices! Expected: %d. Given: %d.", CV_32SC1, _indices.getMat().type());
    	CV_Error(CV_StsBadArg, error_message);
    }
    Mat src = _src.getMat();
    vector<int> indices = _indices.getMat();
    _dst.create(src.rows, src.cols, src.type());
    Mat dst = _dst.getMat();
    for (size_t idx = 0; idx < indices.size(); idx++) {
        Mat originalRow = src.row(indices[idx]);
        Mat sortedRow = dst.row(idx);
        originalRow.copyTo(sortedRow);
    }
}

Mat libfacerec::sortMatrixRowsByIndices(InputArray src, InputArray indices) {
   Mat dst;
   sortMatrixRowsByIndices(src, indices, dst);
   return dst;
}

//------------------------------------------------------------------------------
// libfacerec::asRowMatrix
//------------------------------------------------------------------------------
Mat libfacerec::asRowMatrix(InputArrayOfArrays src, int rtype, double alpha, double beta) {
    // make sure the input data is a vector of matrices or vector of vector
    if(src.kind() != _InputArray::STD_VECTOR_MAT && src.kind() != _InputArray::STD_VECTOR_VECTOR) {
        CV_Error(CV_StsBadArg, "The data is expected as InputArray::STD_VECTOR_MAT (a std::vector<Mat>) or _InputArray::STD_VECTOR_VECTOR (a std::vector< vector<...> >).");
    }
    // number of samples
    size_t n = src.total();
    // return empty matrix if no matrices given
    if(n == 0)
        return Mat();
    // dimensionality of (reshaped) samples
    size_t d = src.getMat(0).total();
    // create data matrix
    Mat data(n, d, rtype);
    // now copy data
    for (size_t i = 0; i < n; i++) {
        // make sure data can be reshaped, throw exception if not!
        if(src.getMat(i).total() != d) {
            string error_message = format("Wrong number of elements in matrix #%d! Expected %d was %d.", i, d, src.getMat(i).total());
            CV_Error(CV_StsBadArg, error_message);
        }
        // get a hold of the current row
        Mat xi = data.row(i);
        // make reshape happy by cloning for non-continuous matrices
        if(src.getMat(i).isContinuous()) {
            src.getMat(i).reshape(1, 1).convertTo(xi, rtype, alpha, beta);
        } else {
            src.getMat(i).clone().reshape(1, 1).convertTo(xi, rtype, alpha, beta);
        }
    }
    return data;
}

//------------------------------------------------------------------------------
// libfacerec::asColumnMatrix
//------------------------------------------------------------------------------
Mat libfacerec::asColumnMatrix(InputArrayOfArrays src, int rtype, double alpha, double beta) {
    // make sure the input data is a vector of matrices or vector of vector
    if(src.kind() != _InputArray::STD_VECTOR_MAT && src.kind() != _InputArray::STD_VECTOR_VECTOR) {
        CV_Error(CV_StsBadArg, "The data is expected as InputArray::STD_VECTOR_MAT (a std::vector<Mat>) or _InputArray::STD_VECTOR_VECTOR (a std::vector< vector<...> >).");
    }
    int n = (int) src.total();
    // return empty matrix if no data given
    if(n == 0)
        return Mat();
    // dimensionality of samples
    int d = src.getMat(0).total();
    // create data matrix
    Mat data(d, n, rtype);
    // now copy data
    for(int i = 0; i < n; i++) {
        // make sure data can be reshaped, throw exception if not!
        if(src.getMat(i).total() != d) {
            string error_message = format("Wrong number of elements in matrix #%d! Expected %d was %d.", i, d, src.getMat(i).total());
            CV_Error(CV_StsBadArg, error_message);
        }
        // get a hold of the current row
        Mat yi = data.col(i);
        // make reshape happy by cloning for non-continuous matrices
        if(src.getMat(i).isContinuous()) {
            src.getMat(i).reshape(1, d).convertTo(yi, rtype, alpha, beta);
        } else {
            src.getMat(i).clone().reshape(1, d).convertTo(yi, rtype, alpha, beta);
        }
    }
    return data;
}
