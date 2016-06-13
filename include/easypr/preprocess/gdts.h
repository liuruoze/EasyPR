//	生成GDTS的文件
//	在捐赠数据到GDTS数据集里，请先用这里的方法对图像进行预处理
//	EasyPR开源项目非常注重保护原始图片的版权
//  所有的捐赠数据通过GDSL协议进行约定，保证使用人仅用于非商业性目的

#ifndef EASYPR_CORE_GDTS_H_
#define EASYPR_CORE_GDTS_H_

#include <opencv2/opencv.hpp>

namespace easypr {

namespace preprocess {

// EasyPR的图像预处理函数，进行模糊化与裁剪化处理

cv::Mat imageProcess(cv::Mat src);

int generate_gdts();
}
}

#endif  // EASYPR_CORE_GDTS_H_