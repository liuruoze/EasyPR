// 数据预处理的程序，主要做以下两件事
// 1.读取原生数据 rawdata，这可能有数百万张
// 2.随机/选择性地选取部分数据成为learndata，这个根据参数设置，一般设置为1000，10000，或者1百万

#include "easypr/preprocess/mc_data.h"
#include <ctime>
#include "easypr/core/plate_judge.h"
#include "easypr/core/plate_locate.h"
#include "easypr/util/util.h"

#ifdef OS_WINDOWS
#include <io.h>
#endif

namespace easypr {

namespace preprocess {

// std::map<std::string, std::string> mc_map = {
//        {"E00", "未识别"},
//        {"A01", "京"},
//        {"A02", "津"},
//        {"A03", "沪"},
//        {"A04", "渝"},
//        {"B01", "桂"},
//        {"B02", "蒙"},
//        {"B03", "宁"},
//        {"B04", "新"},
//        {"B05", "藏"},
//        {"S01", "皖"},
//        {"S02", "闽"},
//        {"S03", "粤"},
//        {"S04", "甘"},
//        {"S05", "贵"},
//        {"S06", "鄂"},
//        {"S07", "冀"},
//        {"S08", "黑"},
//        {"S09", "湘"},
//        {"S10", "豫"},
//        {"S11", "琼"},
//        {"S12", "吉"},
//        {"S13", "苏"},
//        {"S14", "赣"},
//        {"S15", "辽"},
//        {"S16", "青"},
//        {"S17", "川"},
//        {"S18", "鲁"},
//        {"S19", "陕"},
//        {"S20", "晋"},
//        {"S21", "云"},
//        {"S22", "浙"},
//        {"J01", "军"},
//        {"J02", "海"},
//        {"J03", "空"},
//        {"J04", "北"},
//        {"J05", "成"},
//        {"J06", "广"},
//        {"J07", "济"},
//        {"J08", "兰"},
//        {"J09", "南"},
//        {"J10", "沈"}
//};

// 切去上部与底部干扰的细节

cv::Mat cut_top_bottom(const cv::Mat& img) {
  int width = img.size().width;
  int height = img.size().height;
  // TODO: it seems not correctly.
  cv::Rect rect(0, 0, width, int(height * 0.97));
  return img(rect);
}

// std::string code_to_province(const std::string& code) {
//  return (mc_map.find(code) != mc_map.end()) ? mc_map[code] : "无";
//}

//// 通过filepath获取车牌号码
//// 文件名格式：A01_00000
// std::string plate_from_path(const std::string& path) {
//  auto filename = Utils::getFileName(path);
//  auto code = filename.substr(0, 3);
//  return code_to_province(code) + filename.substr(3);
//}

// 将rawdata截取部分数据到learndata中

void create_learn_data(const char* raw_data_folder, const char* out_data_folder,
                       const int how_many /* = 5000 */) {
  assert(raw_data_folder);
  assert(out_data_folder);

  auto files = Utils::getFiles(raw_data_folder);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "No file found in " << raw_data_folder << std::endl;
    return;
  }

  // 随机排列rawdata

  srand(unsigned(time(NULL)));
  std::random_shuffle(files.begin(), files.end());

  int count = 0;
  for (auto f : files) {

    // 选取前how_many个rawdata数据作为learndata

    if (count++ >= how_many) {
      break;
    }

    //读取数据，并对图片进行预处理

    cv::Mat img = cv::imread(f);
    img = cut_top_bottom(img);

    std::string save_to(out_data_folder);
    if (*(save_to.end() - 1) != '/') {
      save_to.push_back('/');
    }
    save_to.append(Utils::getFileName(f, true));

    utils::imwrite(save_to, img);
    std::cout << f << " -> " << save_to << std::endl;
  }
  std::cout << "Learn data created successfully!" << std::endl;
}

// 定位并判断车牌有无，放置在指定位置

void tag_data(const char* source_folder, const char* has_plate_folder,
              const char* no_plate_folder, const char* svm_model) {
  assert(source_folder);
  assert(has_plate_folder);
  assert(no_plate_folder);
  assert(svm_model);

  auto files = Utils::getFiles(source_folder);

  size_t size = files.size();
  if (0 == size) {
    std::cout << "No file found in " << source_folder << std::endl;
    return;
  }

  CPlateLocate locator;

  for (auto f : files) {
    auto filename = Utils::getFileName(f);
    std::cout << "Tagging: " << f << std::endl;

    // auto plate_string = plate_from_path(f);
    cv::Mat image = cv::imread(f);
    assert(!image.empty());

    std::vector<cv::Mat> maybe_plates;
    locator.plateLocate(image, maybe_plates);

    int plate_index = 0;
    for (auto plate : maybe_plates) {
      char save_to[255] = {0};
      int result = 0;
      PlateJudge::instance()->plateJudge(plate, result);
      if (result == 1) {
        // it's a plate
        sprintf(save_to, "%s/%s_%d.jpg", has_plate_folder, filename.c_str(),
                plate_index);
        std::cout << "[Y] -> " << save_to << std::endl;
      } else {
        // no plate found
        sprintf(save_to, "%s/%s_%d.jpg", no_plate_folder, filename.c_str(),
                plate_index);
        std::cout << "[N] -> " << save_to << std::endl;
      }
      utils::imwrite(save_to, plate);
      ++plate_index;
    }
  }
}

}  // namespace preprocess

}  // namespace easypr