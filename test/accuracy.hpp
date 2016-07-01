#ifndef EASYPR_ACCURACY_HPP
#define EASYPR_ACCURACY_HPP

#include <easypr.h>
#include <ctime>
#include <fstream>
#include <list>
#include <memory>
#include <numeric>
#include "easypr/core/core_func.h"
#include "easypr/util/util.h"
#include "thirdparty/xmlParser/xmlParser.h"
#include "easypr/core/params.h"
#include "config.hpp"
#include "result.hpp"

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

    int accuracyTest(const char* test_path, Result& result = Result(), bool useParams = false) {
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
      pr.setResultShow(false);
      pr.setLifemode(true);
      // 设置要处理的一张图片中最多有多少车牌
      pr.setMaxPlates(4);
      //pr.setDetectType(PR_DETECT_COLOR | PR_DETECT_SOBEL);
      pr.setDetectType(PR_DETECT_CMSER);

      //CharsIdentify::instance()->LoadModel(kDefaultAnnPath);
      //PlateJudge::instance()->LoadModel(kDefaultSvmPath);

      //CPlateDetect pd;
      //pd.setDetectType(PR_DETECT_CMSER);
      ////pd.setPDDebug(true);
      //pd.setDetectShow(true);
      //pd.setPDLifemode(true);

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

      int count_nodetect = 0;
      int count_norecogn = 0;

      std::list<std::string> not_recognized_files;

      // all the ground-truth plates
      float all_plate_count = 0;

      // all the characters are right
      float non_error_count = 0;
      float non_error_rate = 0;

      // only one character is wrong
      float one_error_count = 0;
      float one_error_rate = 0;

      // chinese character is wrong
      float chinese_error_count = 0;
      float chinese_error_rate = 0;

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
        int result = pr.plateRecognize(src, plateVec, i);
        //int result = pd.plateDetect(src, plateVec, i);

        // get the ground truth and compare it with the detect list;
        map<string, vector<CPlate>>::iterator it;
        vector<CPlate> plateVecGT;
        it = xmlMap.find(plateLicense);
        if (it != xmlMap.end()) {
          //cout << it->first << endl;
          plateVecGT = it->second;        
        }
        else {
          cout << "No ground truth found!" << endl;
        }
        
        // calucate the detect recall
        // use icdar 2003 evalution protoocal
        vector<float> icdar2003_recall;
        vector<float> icdar2003_precise;

        for (auto plate_g : plateVecGT) {
          float bestmatch = 0.f;
          CPlate* matchPlate = NULL;

          RotatedRect platePos_g = plate_g.getPlatePos();
          Rect_<float> plateRect_g;
          calcSafeRect(platePos_g, src, plateRect_g);

          size_t t = 0;
          for (CPlate plate_d : plateVec) {
            RotatedRect platePos_d = plate_d.getPlatePos();
            Rect_<float> plateRect_d;
            calcSafeRect(platePos_d, src, plateRect_d);

            Rect interRect = plateRect_g & plateRect_d;

            float match = 2 * (interRect.area()) / (plateRect_g.area() + plateRect_d.area());
            if (match - bestmatch > 0.1f) {
              bestmatch = match;
              matchPlate = &(plateVec.at(t));
            } 
            t++;
          }

          icdar2003_recall_all.push_back(bestmatch);
          icdar2003_recall.push_back(bestmatch);

          string plateLicense = plate_g.getPlateStr();
          string license = Utils::splitString(plateLicense, ':').at(1);
          cout << plate_g.getPlateStr() << " (g)" << endl;

          all_plate_count++;

          if (matchPlate && bestmatch > 0.5f) {
            string matchPlateLicense = matchPlate->getPlateStr();
            vector<string> spilt_plate = Utils::splitString(matchPlateLicense, ':');

            size_t size = spilt_plate.size();
            if (size == 2 && spilt_plate.at(1) != "") {
              string matchLicense = spilt_plate.at(1);

              cout << matchPlateLicense << " (d)" << endl;
             
              int diff = utils::levenshtein_distance(license, matchLicense);
              if (diff == 0) {
                non_error_count++;
                one_error_count++;
              }
              else if (diff == 1) {
                one_error_count++;
              }
              cout << kv->get("diff") << ":" << diff << kv->get("char");
              bool chineseError = (license.substr(0, 2) != matchLicense.substr(0, 2));
              if (chineseError) {
                chinese_error_count++;
                //if (diff == 2) {
                //  one_error_count++;
                //}
                // output chinese errror mat
                vector<CCharacter> charVec = matchPlate->getCopyOfReutCharacters();
                CCharacter character = charVec.at(0);
                if (0) {
                  std::stringstream ss(std::stringstream::in | std::stringstream::out);
                  ss << "resources/image/tmp/chinese" << "/" << i << "_" << t << "_" << character.getCharacterStr() << ".jpg";
                  imwrite(ss.str(), character.getCharacterMat());
                }
              }
              cout << "  chineseError:" << chineseError << endl;
            } 
            else {
              cout << "No string" << " (d)" << endl;
              count_norecogn++;
            }
          }
          else {
            cout << kv->get("empty_plate") << endl;
            if (license != kv->get("empty_plate")) {
              not_recognized_files.push_back(license);
              count_nodetect++;
            }
          }

          /* REMAIN
          if (1)
          {
            std::stringstream ss(std::stringstream::in | std::stringstream::out);
            ss << "resources/image/tmp/plate_" << license << ".jpg";
            Mat outMat = src(plateRect_g);
            Mat plate_mat;
            plate_mat.create(36, 136, 16);
            resize(outMat, plate_mat, plate_mat.size(), 0, 0, INTER_AREA);
            imwrite(ss.str(), plate_mat);
          }*/
        }

        // calucate the detect precise
        // use icdar 2003 evalution protoocal
        for (auto plate_d : plateVec) {
          float bestmatch = 0.f;

          RotatedRect platePos_d = plate_d.getPlatePos();
          Rect_<float> plateRect_d;
          calcSafeRect(platePos_d, src, plateRect_d);

          XMLNode rectangleNode = rectangleNodes.addChild("taggedRectangle");
          RotatedRect rr = platePos_d;
          LocateType locateType = plate_d.getPlateLocateType();

          rectangleNode.addAttribute("x", to_string((int)rr.center.x).c_str());
          rectangleNode.addAttribute("y", to_string((int)rr.center.y).c_str());
          rectangleNode.addAttribute("width", to_string((int)rr.size.width).c_str());
          rectangleNode.addAttribute("height", to_string((int)rr.size.height).c_str());

          rectangleNode.addAttribute("rotation", to_string((int)rr.angle).c_str());
          rectangleNode.addAttribute("locateType", to_string(locateType).c_str());
          rectangleNode.addText(plate_d.getPlateStr().c_str());

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

        count_all++;
      }
      time(&end);

      // the xml detection result 
      xMainNode.writeToFile(path_result.c_str());

      cout << "------------------" << endl;
      cout << "Easypr accuracy test end!" << endl;
      cout << "------------------" << endl;
      cout << endl;
      cout << kv->get("summaries") << ":" << endl;
      cout << kv->get("sum_pictures") << ":" << count_all << ",  ";
      cout << "Plates count" << ":" << all_plate_count << ",  ";

      float count_detect = float(all_plate_count - count_nodetect);
      float count_rate = count_detect / all_plate_count;
      cout << kv->get("locate_rate") << ":" << count_rate * 100 << "%  " << endl;

      float count_recogin = float(count_detect - count_norecogn);

      if (count_recogin > 0) {
        non_error_rate = non_error_count / count_recogin;
        one_error_rate = one_error_count / count_recogin;
        chinese_error_rate = chinese_error_count / count_recogin;
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

      //cout << "Detect quality evalution result:" << endl;
      cout << "Recall" << ":" << recall_2003_result * 100 << "%" << ", ";
      cout << "Precise" << ":" << precise_2003_result * 100 << "%" << ", ";
      cout << "Fscore" << ":" << fscore_2003_result * 100 << "%" << "." << endl;

      cout << "0-error" << ":" << non_error_rate * 100 << "%,  ";
      cout << "1-error" << ":" << one_error_rate * 100 << "%,  ";
      cout << "Chinese-precise" << ":" << (1 - chinese_error_rate) * 100 << "%  " << endl;

      double seconds = difftime(end, begin);
      double avgsec = seconds / double(count_all);

      cout << kv->get("seconds") << ":" << seconds << kv->get("sec") << ",  ";
      cout << kv->get("seconds_average") << ":" << avgsec << kv->get("sec") << endl;

      /* REMAIN
      cout << kv->get("unrecognized") << ":" << endl;
      for (auto it = not_recognized_files.begin(); it != not_recognized_files.end();
        ++it) {
        cout << *it << endl;
      }
      cout << endl;
      */

      // set the result.
      if (useParams) {
        result.setPicturesCount(count_all);
        result.setPlatesCount((int)all_plate_count);
        result.setDetectRate((float)count_rate * 100);

        result.setDetectRecall((float)recall_2003_result * 100);
        result.setDetectPrecise((float)precise_2003_result * 100);
        result.setDetectFscore((float)fscore_2003_result * 100);

        result.setZeroError((float)non_error_rate * 100);
        result.setOneError((float)one_error_rate * 100);
        result.setChinesePreciese((float)(1 - chinese_error_rate) * 100);

        result.setSeconds((float)seconds);
        result.setAvgSeconds((float)avgsec);
      }
      
      cout << "------------------" << endl;

      ofstream myfile("result/accuracy.txt", ios::app);
      if (myfile.is_open()) {
        time_t t = time(0);  // get time now
        struct tm* now = localtime(&t);
        char buf[80];

        strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
        myfile << string(buf) << endl;
       
        myfile << kv->get("sum_pictures") << ":" << count_all << ",  ";
        myfile << "Plates count" << ":" << all_plate_count << ",  ";
        myfile << kv->get("unrecognized") << ":" << count_nodetect << ",  ";
        myfile << kv->get("locate_rate") << ":" << count_rate * 100 << "%  "
          << endl;

        myfile << "Recall" << ":" << recall_2003_result * 100 << "%" << ", ";
        myfile << "Precise" << ":" << precise_2003_result * 100 << "%" << ", ";
        myfile << "Fscore" << ":" << fscore_2003_result * 100 << "%" << "." << endl;

        myfile << "0-error" << ":" << non_error_rate * 100 << "%,  ";
        myfile << "1-error" << ":" << one_error_rate * 100 << "%,  ";
        myfile << "Chinese-precise" << ":" << (1 - chinese_error_rate) * 100 << "%  " << endl;
        myfile << kv->get("seconds") << ":" << seconds << kv->get("sec") << ",  ";
        myfile << kv->get("seconds_average") << ":" << avgsec << kv->get("sec") << endl;
        myfile.close();
      }
      else {
        cout << "Unable to open file";
      }
      return 0;
    }

    int gridSearchTest(const char* test_path) {

      std::vector<Result> all_results;

      bool b1[] = {true};
      int b1_c = 1;

      float f1[] = {0.1f, 0.2f, 0.3f};
      int f1_c = 3;

      Config config;
      config.setParam1b(b1, b1_c);
      config.setParam1f(f1, f1_c);

      for (size_t i1 = 0; i1 < config.getParam1b().size(); i1++) {
        bool b1 = config.getParam1b().at(i1);

        for (size_t i2 = 0; i2 < config.getParam1f().size(); i2++) {
          float f1 = config.getParam1f().at(i2);

          CParams::instance()->setParam1b(b1);
          CParams::instance()->setParam1f(f1);

          Result result;
          accuracyTest(test_path, result, true);

          result.getParams().setParam1b(b1);
          result.getParams().setParam1f(f1);

          all_results.push_back(result);
        }
      }

      std::sort(all_results.begin(), all_results.end(),
        [](const Result& r1, const Result& r2) {
        return r1.getChinesePreciese() > r2.getChinesePreciese();
      });

      for (auto result : all_results) {
        std::cout << result << std::endl;
 
        ofstream myfile("result/gridSearch.txt", ios::app);
        if (myfile.is_open()) {
          time_t t = time(0);  // get time now
          struct tm* now = localtime(&t);
          char buf[80];

          strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
          myfile << string(buf) << endl;
          myfile << result << std::endl;
          myfile.close();
        }
      }
      
      return 0;
    }



  }



}

#endif  // EASYPR_ACCURACY_HPP
