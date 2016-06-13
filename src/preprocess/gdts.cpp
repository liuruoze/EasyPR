#include "easypr/preprocess/gdts.h"
#include "easypr/preprocess/deface.h"
#include "easypr/util/util.h"

namespace easypr {

namespace preprocess {

// TODO 将下面的路径改成你的原始图片路径
// 图片不要多，10-30张就足够了，EasyPR对GDTS数据集的使用不以量为主要指标
// 只要这些图片足够反映你数据集的主要特征即可

const char* src_path = "F:/data/easypr-data/tmp-1";

// TODO 将下面的路径改成你希望生成捐赠给GDTS数据存放的新路径

const char* dst_path = "F:/data/easypr-data/tmp-2";

int generate_gdts() {

  // 获取人脸识别文件

  cv::CascadeClassifier cascade;
  std::string cascadeName =
      "resources/model/haarcascade_frontalface_alt_tree.xml";

  ////获取该路径下的所有文件

  auto files = Utils::getFiles(src_path);
  size_t size = files.size();

  if (0 == size) {
    std::cout << "No File Found!" << std::endl;
    return 0;
  }

  std::cout << "Begin to prepare generate_gdts!" << std::endl;

  for (size_t i = 0; i < size; i++) {
    std::string filepath = files[i].c_str();
    std::cout << "------------------" << std::endl;
    std::cout << filepath << std::endl;

    // EasyPR读取原图片

    cv::Mat src = cv::imread(filepath);

    // EasyPR开始对图片进行模糊化与裁剪化处理

    cv::Mat img = imageProcess(src);

    // EasyPR开始对图片进行人脸识别处理

    cv::Mat dst = detectAndMaskFace(img, cascade, 1.5);

    // 将图片导出到新路径

    std::vector<std::string> spilt_path = Utils::splitString(filepath, '\\');

    size_t spiltsize = spilt_path.size();
    std::string filename = "";

    if (spiltsize != 0) filename = spilt_path[spiltsize - 1];

    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << dst_path << "/" << filename;
    utils::imwrite(ss.str(), dst);
  }

  return 0;
}

// EasyPR的图像预处理函数，进行模糊化与裁剪化处理

cv::Mat imageProcess(cv::Mat img) {
  int width = img.size().width;
  int height = img.size().height;
  cv::Rect_<double> rect(width * 0.01, height * 0.01, width * 0.99,
                         height * 0.99);

  cv::Mat dst = img(rect);
  // GaussianBlur( dst, dst, Size(1, 1), 0, 0, BORDER_DEFAULT );
  return dst;
}
}
}
