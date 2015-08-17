//  人脸马赛克
//
//  Created by zhou shiwei on 15/1/20.
//  Copyright (c) 2015年 zhou shiwei. All rights reserved.

//	反人脸识别文件
//	在发布数据到general_test数据集里，请先用这里的方法对图像进行处理
//	EasyPR开源项目非常注重保护图片中驾驶人的隐私

#ifndef EASYPR_CORE_DEFACE_H_
#define EASYPR_CORE_DEFACE_H_

#include <opencv2/opencv.hpp>

namespace easypr {

namespace preprocess {

int deface();

cv::Mat detectAndMaskFace(cv::Mat& img, cv::CascadeClassifier& cascade,
                          double scale);
}
}

#endif  // EASYPR_CORE_DEFACE_H_
