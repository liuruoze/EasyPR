#ifndef EASYPR_ACCURACY_HPP
#define EASYPR_ACCURACY_HPP

#include <ctime>
#include <fstream>

namespace easypr {

namespace demo {

int accuracyTest(const char* test_path) {
  auto files = Utils::getFiles(test_path);

  CPlateRecognize pr;

  pr.LoadSVM("resources/model/svm.xml");
  pr.LoadANN("resources/model/ann.xml");
  pr.setLifemode(true);
  pr.setDebug(false);

  // 设置要处理的一张图片中最多有多少车牌
  pr.setMaxPlates(4);

  size_t files_num = files.size();

  if (0 == files_num) {
    std::cout << "No File Found in " << test_path << "!" << std::endl;
    return 0;
  }

  std::cout << "Begin to test the easypr accuracy!" << std::endl;

  // 总的测试图片数量
  int count_all = 0;
  // 错误的图片数量
  int count_err = 0;
  // 未识别的图片数量
  int count_norecogin = 0;

  // 总的字符差距
  float diff_all = 0;
  // 平均字符差距
  float diff_avg = 0;
  // 完全匹配的识别次数
  float match_count = 0;
  // 完全匹配的识别次数所占识别图片中的比例
  float match_rate = 0;

  // 开始和结束时间
  time_t begin, end;
  time(&begin);

  for (int i = 0; i < files_num; i++) {
    std::string filepath = files[i];
    std::cout << "------------------" << std::endl;

    // 获取真实的车牌
    std::string plateLicense = Utils::getFileName(filepath);
    std::cout << "原牌:" << plateLicense << std::endl;

    // EasyPR开始判断车牌
    cv::Mat src = cv::imread(filepath);

    std::vector<std::string> plateVec;
    int result = pr.plateRecognize(src, plateVec);
    if (result == 0) {
      size_t num = plateVec.size();

      if (num == 0) {
        std::cout << "无车牌" << std::endl;
        if (plateLicense != "无车牌") count_norecogin++;
      } else if (num > 1) {
        // 多车牌使用diff最小的那个记录
        int mindiff = 10000;
        for (int j = 0; j < num; j++) {
          std::cout << plateVec[j] << " (" << j + 1 << ")" << std::endl;
          std::string colorplate = plateVec[j];

          // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"
          auto spilt_plate = Utils::splitString(colorplate, ':');

          size_t size = spilt_plate.size();
          if (size == 2 && spilt_plate[1] != "") {
            int diff =
                    Utils::levenshtein_distance(plateLicense,
                                                spilt_plate[size - 1]);
            if (diff < mindiff) mindiff = diff;
          }
        }

        std::cout << "差距:" << mindiff << "个字符" << std::endl;
        if (mindiff == 0) {
          // 完全匹配
          match_count++;
        }
        diff_all = diff_all + mindiff;
      } else {
        // 单车牌只计算一次diff
        for (int j = 0; j < num; j++) {
          std::cout << plateVec[j] << std::endl;
          std::string colorplate = plateVec[j];

          // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"
          std::vector<std::string> spilt_plate = Utils::splitString(colorplate,
                                                                    ':');

          size_t size = spilt_plate.size();
          if (size == 2 && spilt_plate[1] != "") {
            int diff =
                    Utils::levenshtein_distance(plateLicense,
                                                spilt_plate[size - 1]);
            std::cout << "差距:" << diff << "个字符" << std::endl;

            if (diff == 0) {
              // 完全匹配
              match_count++;
            }
            diff_all = diff_all + diff;
          }
        }
      }
    } else {
      std::cout << "错误码:" << result << std::endl;
      count_err++;
    }
    count_all++;
  }
  time(&end);

  std::cout << "------------------" << std::endl;
  std::cout << "Easypr accuracy test end!" << std::endl;
  std::cout << "------------------" << std::endl;
  std::cout << std::endl;
  std::cout << "统计参数:" << std::endl;
  std::cout << "总图片数:" << count_all << "张,  ";
  std::cout << "未识出图片:" << count_norecogin << "张,  ";

  float count_recogin = count_all - (count_err + count_norecogin);
  float count_rate = count_recogin / count_all;

  std::cout << "定位率:" << count_rate * 100 << "%  " << std::endl;

  diff_avg = diff_all / count_recogin;
  match_rate = match_count / count_recogin * 100;

  std::cout << "平均字符差距:" << diff_avg << "个,  ";
  std::cout << "完全匹配数:" << match_count << "张,  ";
  std::cout << "完全匹配率:" << match_rate << "%  " << std::endl;

  double seconds = difftime(end, begin);
  double avgsec = seconds / double(count_all);

  std::cout << "总时间:" << seconds << "秒,  ";
  std::cout << "平均执行时间:" << avgsec << "秒  " << std::endl;

  std::cout << std::endl;

  std::cout << "------------------" << std::endl;

  std::ofstream myfile("accuracy.txt", std::ios::app | std::ios::out);
  if (myfile.is_open()) {
    time_t t = time(0);  // get time now
    struct tm* now = localtime(&t);
    char buf[80];

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
    myfile << std::string(buf) << std::endl;

    myfile << "总图片数:" << count_all << "张,  ";
    myfile << "未识出图片:" << count_norecogin << "张,  ";
    myfile << "定位率:" << count_rate * 100 << "%  " << std::endl;
    myfile << "平均字符差距:" << diff_avg << "个,  ";
    myfile << "完全匹配数:" << match_count << "张,  ";
    myfile << "完全匹配率:" << match_rate << "%  " << std::endl;
    myfile << "总时间:" << seconds << "秒,  ";
    myfile << "平均执行时间:" << avgsec << "秒" << std::endl;
    myfile.close();
  } else {
    std::cout << "Unable to open file";
  }
  return 0;
}

}

}

#endif //EASYPR_ACCURACY_HPP
