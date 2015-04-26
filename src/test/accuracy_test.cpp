// 通用正确率测试文件
// AcurayTest对应到main控制命令中的选项2

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/feature.h"

using namespace easypr;

int acurayTest(const string& test_path) {
    ////获取该路径下的所有文件
    auto files = Utils::getFiles(test_path);
    
    // CPlateLocate lo;
    // CPlateJudge ju;
    CPlateRecognize pr;
    
    pr.LoadANN("model/ann.xml");
    pr.LoadSVM("model/svm.xml");
    pr.setLifemode(true);
    pr.setDebug(false);
    
    // 设置要处理的一张图片中最多有多少车牌
    pr.setMaxPlates(4);
    
    // CPlateDetect pd;
    // pd.LoadSVM("model/svm.xml");
    // pd.setPDLifemode(true);
    
    int size = files.size();
    // int size = 200;
    
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
    
    for (int i = 0; i < size; i++) {
        string filepath = files[i].c_str();
        cout << "------------------" << endl;
        
        // 获取真实的车牌
        string plateLicense = Utils::getFileName(filepath);
        cout << "原牌:" << plateLicense << endl;
        
        // EasyPR开始判断车牌
        Mat src = imread(filepath);
        
        vector<string> plateVec;
        int result = pr.plateRecognize(src, plateVec, i);
        if (result == 0) {
            int num = plateVec.size();
            
            if (num == 0) {
                cout << "无车牌" << endl;
                if (plateLicense != "无车牌") count_norecogin++;
            } else if (num > 1) {
                // 多车牌使用diff最小的那个记录
                int mindiff = 10000;
                for (int j = 0; j < num; j++) {
                    cout << plateVec[j] << " (" << j + 1 << ")" << endl;
                    string colorplate = plateVec[j];
                    
                    // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"
                    vector<string> spilt_plate = Utils::splitString(colorplate, ':');
                    
                    int size = spilt_plate.size();
                    if (size == 2 && spilt_plate[1] != "") {
                        int diff =
                        levenshtein_distance(plateLicense, spilt_plate[size - 1]);
                        if (diff < mindiff) mindiff = diff;
                    }
                }
                
                cout << "差距:" << mindiff << "个字符" << endl;
                if (mindiff == 0) {
                    // 完全匹配
                    match_count++;
                }
                diff_all = diff_all + mindiff;
            } else {
                // 单车牌只计算一次diff
                for (int j = 0; j < num; j++) {
                    cout << plateVec[j] << endl;
                    string colorplate = plateVec[j];
                    
                    // 计算"蓝牌:苏E7KU22"中冒号后面的车牌大小"
                    vector<string> spilt_plate = Utils::splitString(colorplate, ':');
                    
                    int size = spilt_plate.size();
                    if (size == 2 && spilt_plate[1] != "") {
                        int diff =
                        levenshtein_distance(plateLicense, spilt_plate[size - 1]);
                        cout << "差距:" << diff << "个字符" << endl;
                        
                        if (diff == 0) {
                            // 完全匹配
                            match_count++;
                        }
                        diff_all = diff_all + diff;
                    }
                }
            }
        } else {
            cout << "错误码:" << result << endl;
            count_err++;
        }
        count_all++;
    }
    time(&end);
    
    cout << "------------------" << endl;
    cout << "Easypr accuracy test end!" << endl;
    cout << "------------------" << endl;
    cout << endl;
    cout << "统计参数:" << endl;
    cout << "总图片数:" << count_all << "张,  ";
    cout << "未识出图片:" << count_norecogin << "张,  ";
    
    float count_recogin = count_all - (count_err + count_norecogin);
    float count_rate = count_recogin / count_all;
    //float count_norate = 1 - count_rate;
    cout << "定位率:" << count_rate * 100 << "%  " << endl;
    
    diff_avg = diff_all / count_recogin;
    match_rate = match_count / count_recogin * 100;
    
    cout << "平均字符差距:" << diff_avg << "个,  ";
    cout << "完全匹配数:" << match_count << "张,  ";
    cout << "完全匹配率:" << match_rate << "%  " << endl;
    
    double seconds = difftime(end, begin);
    double avgsec = seconds / double(count_all);
    
    cout << "总时间:" << seconds << "秒,  ";
    cout << "平均执行时间:" << avgsec << "秒  " << endl;
    
    cout << endl;
    
    cout << "------------------" << endl;
    
    ofstream myfile("run_accuracy.txt", ios::app);
    if (myfile.is_open()) {
        time_t t = time(0);  // get time now
        struct tm* now = localtime(&t);
        char buf[80];
        
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", now);
        myfile << string(buf) << endl;
        
        myfile << "总图片数:" << count_all << "张,  ";
        myfile << "未识出图片:" << count_norecogin << "张,  ";
        myfile << "定位率:" << count_rate * 100 << "%  " << endl;
        myfile << "平均字符差距:" << diff_avg << "个,  ";
        myfile << "完全匹配数:" << match_count << "张,  ";
        myfile << "完全匹配率:" << match_rate << "%  " << endl;
        myfile << "总时间:" << seconds << "秒,  ";
        myfile << "平均执行时间:" << avgsec << "秒  " << endl;
        myfile.close();
    } else {
        cout << "Unable to open file";
    }
    return 0;
}
