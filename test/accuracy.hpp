#ifndef EASYPR_ACCURACY_HPP
#define EASYPR_ACCURACY_HPP

#include <easypr.h>
#include <ctime>
#include <fstream>
#include <list>
#include <memory>
#include <numeric> 
#include "xml/xmlParser.h"
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"

using namespace std;

namespace easypr {

  namespace demo {

    int getGroundTruth(map<string, vector<CPlate>>& xmlMap, const char* path) {

#ifdef OS_WINDOWS
      XMLNode::setGlobalOptions(XMLNode::char_encoding_GBK);
#endif
      XMLNode xMainNode = XMLNode::openFileHelper(path, "tagset");

      int n = xMainNode.nChildNode("image");

      // this prints the "coefficient" value for all the "NumericPredictor" tags:
      for (int i = 0; i < n; i++) {
        XMLNode imageNode = xMainNode.getChildNode("image", i);
        string imageName = imageNode.getChildNode("imageName").getText();

        vector<CPlate> plateVec;
        int m = imageNode.getChildNode("taggedRectangles").nChildNode("taggedRectangle");
        for (int j = 0; j < m; j++) {
          XMLNode plateNode = imageNode.getChildNode("taggedRectangles").getChildNode("taggedRectangle", j);

          int x = atoi(plateNode.getAttribute("x"));
          int y = atoi(plateNode.getAttribute("y"));
          int width = atoi(plateNode.getAttribute("width"));
          int height = atoi(plateNode.getAttribute("height"));
          int angle = atoi(plateNode.getAttribute("rotation"));

          string plateStr = plateNode.getText();

          if (width < height) {
            std::swap(width, height);
            angle = angle + 90;
          }

          RotatedRect rr(Point2f(float(x), float(y)), Size2f(float(width), float(height)), (float)angle);

          CPlate plate;
          plate.setPlateStr(plateStr);
          plate.setPlatePos(rr);
          plateVec.push_back(plate);
        }

        xmlMap[imageName] = plateVec;
      }

      return 0;
    }



    int accuracyTest(const char* test_path) {
      std::shared_ptr<easypr::Kv> kv(new easypr::Kv);
      kv->load("etc/chinese_mapping");

      map<string, vector<CPlate>> xmlMap;
      string path(test_path);
      path = path + "/GroundTruth.xml";
      getGroundTruth(xmlMap, path.c_str());

      //cout << xmlMap.size() << endl;
      //cout << xmlMap.begin()->first << endl;
      //vector<CPlate> plateVec = xmlMap.begin()->second;
      //cout << plateVec[0].getPlateStr() << endl;

      XMLNode xMainNode = XMLNode::createXMLTopNode("tagset");
#ifdef OS_WINDOWS
      XMLNode::setGlobalOptions(XMLNode::char_encoding_GBK);
#endif
      auto files = Utils::getFiles(test_path);

      std::string path_result = "result/Result.xml";

      CPlateRecognize pr;

      // 设置Debug模式

      pr.setDebug(false);

      pr.setLifemode(true);

      // 设置要处理的一张图片中最多有多少车牌

      pr.setMaxPlates(4);

      pr.setDetectType(PR_DETECT_COLOR | PR_DETECT_SOBEL);
      //pr.setDetectType(PR_DETECT_CMSER);

      int size = files.size();

      if (0 == size) {
        cout << "No File Found in general_test/native_test!" << endl;
        return 0;
      }

      cout << "Begin to test the easypr accuracy!" << endl;

      // 总的测试图片数量

      int count_all = 0;

      // 错误的图片数量

      int count_err = 0;

      // 未识别的图片数量

      int count_norecogin = 0;

      std::list<std::string> not_recognized_files;

      // 总的字符差距

      float diff_all = 0;

      // 平均字符差距

      float diff_avg = 0;

      // 完全匹配的识别次数

      float match_count = 0;

      // 完全匹配的识别次数所占识别图片中的比例

      float match_rate = 0;

      // calucate the detect precise and recall
      // use icdar 2003 evalution protoocal
      vector<float> icdar2003_recall_all;
      vector<float> icdar2003_precise_all;

      // 开始和结束时间

      time_t begin, end;
      time(&begin);

      for (int i = 0; i < size; i++) {
        string filepath = files[i].c_str();

        // EasyPR开始判断车牌

        Mat src = imread(filepath);

        // 如果是非图像文件，直接过去

        if (!src.data) continue;

        cout << "------------------" << endl;

        // 获取真实的车牌

        string plateLicense = Utils::getFileName(filepath);
        cout << kv->get("original_plate") << ":" << plateLicense << endl;

        XMLNode xNode = xMainNode.addChild("image");
        xNode.addChild("imageName").addText(plateLicense.c_str());

        XMLNode rectangleNodes = xNode.addChild("taggedRectangles");

        vector<CPlate> plateVec;
        int result = pr.plateRecognize(src, plateVec);

        // get the ground truth and compare it with the detect list;
        map<string, vector<CPlate>>::iterator it;
        vector<CPlate> plateVecGT;
        it = xmlMap.find(plateLicense);
        if (it != xmlMap.end()) {
          //cout << it->first << endl;
          plateVecGT = it->second;        
        }
        
        // calucate the detect recall
        // use icdar 2003 evalution protoocal
        vector<float> icdar2003_recall;
        vector<float> icdar2003_precise;

        for (auto plate_g : plateVecGT) {
          float bestmatch = 0.f;

          RotatedRect platePos_g = plate_g.getPlatePos();
          Rect_<float> plateRect_g;
          calcSafeRect(platePos_g, src, plateRect_g);

          for (auto plate_d : plateVec) {
            RotatedRect platePos_d = plate_d.getPlatePos();
            Rect_<float> plateRect_d;
            calcSafeRect(platePos_d, src, plateRect_d);

            Rect interRect = plateRect_g & plateRect_d;

            float match = 2 * (interRect.area()) / (plateRect_g.area() + plateRect_d.area());
            if (match > bestmatch)
              bestmatch = match;
          }

          icdar2003_recall_all.push_back(bestmatch);
          icdar2003_recall.push_back(bestmatch);

          string plateLicense = plate_g.getPlateStr();
          string license = Utils::splitString(plateLicense, ':')[1];
          cout << plate_g.getPlateStr() << " (g)" << endl;
          if (1)
          {
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
            ss << "resources/image/tmp/plate_" << license << ".jpg";
            Mat outMat = src(plateRect_g);
            Mat plate_mat;
            plate_mat.create(36, 136, 16);
            resize(outMat, plate_mat, plate_mat.size(), 0, 0, INTER_AREA);
            imwrite(ss.str(), plate_mat);
          }
        }

        // calucate the detect precise
        // use icdar 2003 evalution protoocal
        for (auto plate_d : plateVec) {
          float bestmatch = 0.f;

          RotatedRect platePos_d = plate_d.getPlatePos();
          Rect_<float> plateRect_d;
          calcSafeRect(platePos_d, src, plateRect_d);

          for (auto plate_g : plateVecGT) {
            RotatedRect platePos_g = plate_g.getPlatePos();
            Rect_<float> plateRect_g;
            calcSafeRect(platePos_g, src, plateRect_g);

            Rect interRect = plateRect_g & plateRect_d;

            float match = 2 * (interRect.area()) / (plateRect_g.area() + plateRect_d.area());
            if (match > bestmatch)
              bestmatch = match;
          }

          icdar2003_precise_all.push_back(bestmatch);     
          icdar2003_precise.push_back(bestmatch);
        }

        double recall_result = 0;
        if (icdar2003_recall.size() > 0) {
          recall_result = std::accumulate(icdar2003_recall.begin(),
            icdar2003_recall.end(), 0.0) / (double)icdar2003_recall.size();
        }

        double precise_result = 0;
        if (icdar2003_precise.size() > 0) {
          precise_result = std::accumulate(icdar2003_precise.begin(),
            icdar2003_precise.end(), 0.0) / (double)icdar2003_precise.size();
        }

        double fscore_result = 0;
        if (recall_result + precise_result != 0) {
          fscore_result = 2 * (recall_result * precise_result) /
            (recall_result + precise_result);
        }

        cout << "Recall" << ":" << recall_result * 100 << "%" << ", ";
        cout << "Precise" << ":" << precise_result * 100 << "%" << ", ";
        cout << "Fscore" << ":" << fscore_result * 100 << "%" << "." << endl;

        if (result == 0) {
          int num = plateVec.size();
          if (num == 0) {
            cout << kv->get("empty_plate") << endl;
            if (plateLicense != kv->get("empty_plate")) {
              not_recognized_files.push_back(plateLicense);
              count_norecogin++;
            }
          }
          else if (num > 1) {
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
              rectangleNode.addText(plateVec[j].getPlateStr().c_str());

              string colorplate = plateVec[j].getPlateStr();
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
              match_count++;
            }
            diff_all = diff_all + mindiff;
          }
          else {
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
              rectangleNode.addText(plateVec[j].getPlateStr().c_str());

              string colorplate = plateVec[j].getPlateStr();

              // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"

              vector<string> spilt_plate = Utils::splitString(colorplate, ':');

              int size = spilt_plate.size();
              if (size == 2 && spilt_plate[1] != "") {
                int diff = utils::levenshtein_distance(plateLicense,
                  spilt_plate[size - 1]);
                cout << kv->get("diff") << ":" << diff << kv->get("char") << endl;

                if (diff == 0) {
                  match_count++;
                }
                diff_all = diff_all + diff;
              }
            }
          }
        }
        else {
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

      xMainNode.writeToFile(path_result.c_str());

      float count_recogin = float(count_all - (count_err + count_norecogin));
      float count_rate = count_recogin / count_all;
      cout << kv->get("locate_rate") << ":" << count_rate * 100 << "%  " << endl;

      if (count_recogin > 0) {
        diff_avg = diff_all / count_recogin;
      }

      if (count_recogin > 0) {
        match_rate = match_count / count_recogin * 100;
      }

      double recall_2003_result = 0;
      if (icdar2003_recall_all.size() > 0) {
        recall_2003_result = std::accumulate(icdar2003_recall_all.begin(),
          icdar2003_recall_all.end(), 0.0) / (double)icdar2003_recall_all.size();
      }

      double precise_2003_result = 0;
      if (icdar2003_precise_all.size() > 0) {
        precise_2003_result = std::accumulate(icdar2003_precise_all.begin(),
          icdar2003_precise_all.end(), 0.0) / (double)icdar2003_precise_all.size();
      }

      double fscore_2003_result = 0;
      if (recall_2003_result + precise_2003_result != 0) {
        fscore_2003_result = 2 * (recall_2003_result * precise_2003_result) /
          (recall_2003_result + precise_2003_result);
      }

      cout << "Detect quality evalution result:" << endl;
      cout << "Recall" << ":" << recall_2003_result * 100 << "%" << ", ";
      cout << "Precise" << ":" << precise_2003_result * 100 << "%" << ", ";
      cout << "Fscore" << ":" << fscore_2003_result * 100 << "%" << "." << endl;

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


        myfile << "Recall" << ":" << recall_2003_result * 100 << "%" << ", ";
        myfile << "Precise" << ":" << precise_2003_result * 100 << "%" << ", ";
        myfile << "Fscore" << ":" << fscore_2003_result * 100 << "%" << "." << endl;

        myfile << kv->get("diff_average") << ":" << diff_avg << ",  ";
        myfile << kv->get("full_match") << ":" << match_count << ",  ";
        myfile << kv->get("full_rate") << ":" << match_rate << "%  " << endl;
        myfile << kv->get("seconds") << ":" << seconds << kv->get("sec") << ",  ";
        myfile << kv->get("seconds_average") << ":" << avgsec << kv->get("sec")
          << endl;
        myfile.close();
      }
      else {
        cout << "Unable to open file";
      }
      return 0;
    }

  }
}

#endif  // EASYPR_ACCURACY_HPP
