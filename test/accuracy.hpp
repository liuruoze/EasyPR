#ifndef EASYPR_ACCURACY_HPP
#define EASYPR_ACCURACY_HPP

#include <easypr.h>
#include <ctime>
#include <fstream>
#include <list>
#include <memory>

using namespace std;

#include "xml\xmlParser.h"

namespace easypr {

namespace demo {

int accuracyTest(const char* test_path) {
  std::shared_ptr<easypr::Kv> kv(new easypr::Kv);
  kv->load("etc/chinese_mapping");

  //int n = xMainNode.nChildNode("image");

  //cout << n << endl;
  //
  ////for (int i = 0; i < n; i++) {
  ////  XMLNode xNode = xMainNode.getChildNode("image", i);
  ////  cout << xNode.getChildNode("imageName").getText() << endl;
  ////}

  ////XMLNode xNode = xMainNode.getChildNode("image", 0);
  ////cout << xNode.getChildNode("imageName").getText() << endl;
  ////xNode.getChildNode("imageName").updateText("test1");
  //
  //XMLNode xNode = xMainNode.addChild("image");

  //cout << xNode.getChildNode("imageName").getText() << endl;
  //xNode.getChildNode("imageName").updateText("test1");


  //char *t = xNode.createXMLString(true);
  //printf("%s\n", t);
  //free(t);

  XMLNode xMainNode = XMLNode::createXMLTopNode("tagset");
  XMLNode::setGlobalOptions(XMLNode::char_encoding_GBK);

  auto files = Utils::getFiles(test_path);

  CPlateRecognize pr;

  // 设置Debug模式

  pr.setDebug(false);

  pr.setLifemode(true);

  // 设置要处理的一张图片中最多有多少车牌

  pr.setMaxPlates(4);

  int size = files.size();

  if (0 == size) {
    cout << "No File Found in general_test/native_test!" << endl;
    return 0;
  }

  cout << "Begin to test the easypr accuracy!" << endl;

  // 总的测试图片数量

  int count_all = 0;

  // 错误的图片数�

  int count_err = 0;

  // 未识别的图片数量

  int count_norecogin = 0;

  std::list<std::string> not_recognized_files;

  // 总的字符差距

  float diff_all = 0;

  // 平均字符差距

  float diff_avg = 0;

  // 完全匹配的识别次�

  float match_count = 0;

  // 完全匹配的识别次数所占识别图片中的比�

  float match_rate = 0;

  // 开始和结束时间

  time_t begin, end;
  time(&begin);

  for (int i = 0; i < 10; i++) {
    string filepath = files[i].c_str();

    // EasyPR开始判断车�

    Mat src = imread(filepath);

    // 如果是非图像文件，直接过�

    if (!src.data) continue;

    cout << "------------------" << endl;

    // 获取真实的车�

    string plateLicense = Utils::getFileName(filepath);
    cout << kv->get("original_plate") << ":" << plateLicense << endl;

    XMLNode xNode = xMainNode.addChild("image");
    int result = pr.plateRecognize(src, plateVec);

    XMLNode rectangleNodes = xNode.addChild("taggedRectangles");

    vector<CPlate> plateVec;
    if (result == 0) {
      int num = plateVec.size();

      if (num == 0) {
        cout << kv->get("empty_plate") << endl;
        if (plateLicense != kv->get("empty_plate")) {
          not_recognized_files.push_back(plateLicense);
          count_norecogin++;
        }
      } else if (num > 1) {

        // 多车牌使用diff最小的那个记录

        int mindiff = 10000;
        for (int j = 0; j < num; j++) {
          cout << plateVec[j].getPlateStr() << " (" << j + 1 << ")" << endl;

          XMLNode rectangleNode = rectangleNodes.addChild("taggedRectangle");
          RotatedRect rr = plateVec[j].getPlatePos();
          LocateType locateType = plateVec[j].getPlateLocateType();
          
          rectangleNode.addAttribute("x", to_string((int)rr.center.x).c_str());
          rectangleNode.addAttribute("y", to_string((int)rr.center.y).c_str());
          rectangleNode.addAttribute("width", to_string((int)rr.size.width).c_str());
          rectangleNode.addAttribute("height", to_string((int)rr.size.height).c_str());

          rectangleNode.addAttribute("rotation", to_string((int)rr.angle).c_str());
          rectangleNode.addAttribute("locateType", to_string(locateType).c_str());
          //rectangleNode.addText(plateVec[j].getPlateStr().c_str());
          rectangleNode.addAttribute("text", plateVec[j].getPlateStr().c_str());

          string colorplate = plateVec[j].getPlateStr();

          // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"

          vector<string> spilt_plate = Utils::splitString(colorplate, ':');

          int size = spilt_plate.size();
          if (size == 2 && spilt_plate[1] != "") {
            int diff = utils::levenshtein_distance(plateLicense,
                                                   spilt_plate[size - 1]);
            if (diff < mindiff) mindiff = diff;
          }
        }

        cout << kv->get("diff") << ":" << mindiff << kv->get("char") << endl;
        if (mindiff == 0) {

          // 完全匹配

          match_count++;
        }
        diff_all = diff_all + mindiff;
      } else {

        // 单车牌只计算一次diff

        for (int j = 0; j < num; j++) {
          cout << plateVec[j].getPlateStr() << endl;

          XMLNode rectangleNode = rectangleNodes.addChild("taggedRectangle");
          RotatedRect rr = plateVec[j].getPlatePos();
          LocateType locateType = plateVec[j].getPlateLocateType();

          rectangleNode.addAttribute("x", to_string((int)rr.center.x).c_str());
          rectangleNode.addAttribute("y", to_string((int)rr.center.y).c_str());
          rectangleNode.addAttribute("width", to_string((int)rr.size.width).c_str());
          rectangleNode.addAttribute("height", to_string((int)rr.size.height).c_str());

          rectangleNode.addAttribute("rotation", to_string((int)rr.angle).c_str());
          rectangleNode.addAttribute("locateType", to_string(locateType).c_str());
          //rectangleNode.addText(plateVec[j].getPlateStr().c_str());
          rectangleNode.addAttribute("text", plateVec[j].getPlateStr().c_str());

          string colorplate = plateVec[j].getPlateStr();

          // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"

          vector<string> spilt_plate = Utils::splitString(colorplate, ':');

          int size = spilt_plate.size();
          if (size == 2 && spilt_plate[1] != "") {
            int diff = utils::levenshtein_distance(plateLicense,
                                                   spilt_plate[size - 1]);
            cout << kv->get("diff") << ":" << diff << kv->get("char") << endl;

            if (diff == 0) {

              // 完全匹配

              match_count++;
            }
            diff_all = diff_all + diff;
          }
        }
      }
    } else {
      cout << kv->get("error_code") << ":" << result << endl;
      count_err++;
    }
    count_all++;
  }
  time(&end);

  cout << "------------------" << endl;
  cout << "Easypr accuracy test end!" << endl;
  cout << "------------------" << endl;
  cout << endl;
  cout << kv->get("summaries") << ":" << endl;
  cout << kv->get("sum_pictures") << ":" << count_all << ",  ";
  cout << kv->get("unrecognized") << ":" << count_norecogin << ",  ";

  float count_recogin = float(count_all - (count_err + count_norecogin));
  float count_rate = count_recogin / count_all;
  cout << kv->get("locate_rate") << ":" << count_rate * 100 << "%  " << endl;

  if (count_recogin > 0) {
    diff_avg = diff_all / count_recogin;
  }

  if (count_recogin > 0) {
    match_rate = match_count / count_recogin * 100;
  }

  cout << kv->get("diff_average") << ":" << diff_avg << ",  ";
  cout << kv->get("full_match") << ":" << match_count << ",  ";
  cout << kv->get("full_rate") << ":" << match_rate << "%  " << endl;

  double seconds = difftime(end, begin);
  double avgsec = seconds / double(count_all);

  cout << kv->get("seconds") << ":" << seconds << kv->get("sec") << ",  ";
  cout << kv->get("seconds_average") << ":" << avgsec << kv->get("sec") << endl;

  cout << kv->get("unrecognized") << ":" << endl;

  for (auto it = not_recognized_files.begin(); it != not_recognized_files.end();
       ++it) {
    cout << *it << endl;
  }

  cout << endl;

  cout << "------------------" << endl;

  ofstream myfile("accuracy.txt", ios::app);
  if (myfile.is_open()) {
    time_t t = time(0);  // get time now
    struct tm* now = localtime(&t);
    char buf[80];

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
    myfile << string(buf) << endl;

    myfile << kv->get("sum_pictures") << ":" << count_all << ",  ";
    myfile << kv->get("unrecognized") << ":" << count_norecogin << ",  ";
    myfile << kv->get("locate_rate") << ":" << count_rate * 100 << "%  "
        << endl;
    myfile << kv->get("diff_average") << ":" << diff_avg << ",  ";
    myfile << kv->get("full_match") << ":" << match_count << ",  ";
    myfile << kv->get("full_rate") << ":" << match_rate << "%  " << endl;
    myfile << kv->get("seconds") << ":" << seconds << kv->get("sec") << ",  ";
    myfile << kv->get("seconds_average") << ":" << avgsec << kv->get("sec")
        << endl;
    myfile.close();
  } else {
    cout << "Unable to open file";
  }
  return 0;
}

}
}

#endif  // EASYPR_ACCURACY_HPP
