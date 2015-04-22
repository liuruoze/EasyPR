#include "include/plate_recognize.h"
#include "include/util.h"
#include "include/feature.h"
#include "include/CParser.h"

using namespace easypr;

int svmMain();
int acurayTestMain();
void cmdMain(int argc, char *argv[]);

namespace easypr {

    int svmTrain(bool dividePrepared = true, bool trainPrepared = true,
                 svmCallback getFeatures = getHistogramFeatures);

}

extern const string GENERAL_TEST_PATH = "image/general_test";
extern const string NATIVE_TEST_PATH = "image/native_test";

////////////////////////////////////////////////////////////
// EasyPR 训练命令行

const string option[] =
{
    "1. 测试;"	,
    "2. 批量测试;"		,
    "3. SVM训练;"		,
    "4. ANN训练(未开放);"		,
    "5. GDTS生成;"		,
    "6. 开发团队;"		,
    "7. 感谢名单;"		,
    "8. 退出;"			,
};

const int optionCount = 8;

int main(int argc, char *argv[])
{
    if (argc > 1) {
        //enable command line option
        cmdMain(argc, argv);
        return 0;
    }

    bool isExit = false;
    while (isExit != true)
    {
        stringstream selectOption(stringstream::in | stringstream::out);
        selectOption << "EasyPR Option:"<< endl;
        for(int i = 0; i < optionCount; i++)
        {
            selectOption << option[i] << endl;
        }

        cout << "////////////////////////////////////"<< endl;
        cout << selectOption.str();
        cout << "////////////////////////////////////"<< endl;
        cout << "请选择一项操作:";

        int select = -1;
        bool isRepeat = true;
        while (isRepeat)
        {
            cin >> select;
            isRepeat = false;
            switch (select)
            {
                case 1:
                    testMain();
                    break;
                case 2:
                    acurayTestMain();
                    break;
                case 3:
                    svmMain();
                    break;
                case 4:
                    // TODO
                    break;
                case 5:
                    //general_test();
                    generate_gdts();
                    break;
                case 6:
                    // 开发团队;
                    cout << endl;
                    cout << "我们EasyPR团队目前有一个5人左右的小组在进行EasyPR后续版本的开发工作。" << endl;
                    cout << "如果你对本项目感兴趣，并且愿意为开源贡献一份力量，我们很欢迎你的加入。" << endl;
                    cout << "目前招聘的主要人才是：车牌定位，图像识别，深度学习，网站建设相关方面的牛人。" << endl;
                    cout << "如果你觉得自己符合条件，请发邮件到地址(easypr_dev@163.com)，期待你的加入！" << endl;
                    cout << endl;
                    break;
                case 7:
                    // 感谢名单
                    cout << endl;
                    cout << "本项目在建设过程中，受到了很多人的帮助，其中以下是对本项目做出突出贡献的" << endl;
                    cout << "(贡献包括有益建议，代码调优，数据提供等等,排名按时间顺序)：" << endl;
                    cout << "taotao1233，邱锦山，唐大侠，jsxyhelu，如果有一天(zhoushiwei)，学习奋斗，袁承志，圣城小石匠，goldriver，Micooz，梦里时光，Rain Wang，ahccoms，星夜落尘，海豚嘎嘎" << endl;
                    cout << "还有很多的同学对本项目也给予了鼓励与支持，在此也一并表示真诚的谢意！" << endl;
                    cout << endl;
                    break;
                case 8:
                    isExit = true;
                    break;
                default:
                    cout << "输入错误，请重新输入:";
                    isRepeat = true;
                    break;
            }
        }
    }
    return 0;
}
// /EasyPR 训练命令行 结束
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// acurayTestMain 命令行

const string acuray_option[] =
{
    "1. general_test;"	,
    "2. native_test;"	,
    "3. 返回;"			,
};

const int acuray_optionCount = 3;

int acurayTestMain()
{
    bool isExit = false;
    while (isExit != true)
    {
        stringstream selectOption(stringstream::in | stringstream::out);
        selectOption << "BatchTest Option:"<< endl;
        for(int i = 0; i < acuray_optionCount; i++)
        {
            selectOption << acuray_option[i] << endl;
        }

        cout << "////////////////////////////////////"<< endl;
        cout << selectOption.str();
        cout << "////////////////////////////////////"<< endl;
        cout << "请选择一项操作:";

        int select = -1;
        bool isRepeat = true;
        while (isRepeat)
        {
            cin >> select;
            isRepeat = false;
            switch (select)
            {
                case 1:
                    acurayTest(GENERAL_TEST_PATH);
                    break;
                case 2:
                    acurayTest(NATIVE_TEST_PATH);
                    break;
                case 3:
                    isExit = true;
                    break;
                default:
                    cout << "输入错误，请重新输入:";
                    isRepeat = true;
                    break;
            }
        }
    }
    return 0;
}

// acurayTestMain 命令行
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// SVM 训练命令行

const string svm_option[] =
{
    "1. 生成learndata(调整代码到你的环境后再用);"	,
    "2. 标签learndata;"		,
    "3. 车牌检测(not divide and train);"		,
    "4. 车牌检测(not train);"		,
    "5. 车牌检测(not divide);"		,
    "6. 车牌检测;"		,
    "7. 返回;"			,
};

const int svm_optionCount = 7;

int svmMain()
{
    bool isExit = false;
    while (isExit != true)
    {
        stringstream selectOption(stringstream::in | stringstream::out);
        selectOption << "SvmTrain Option:"<< endl;
        for(int i = 0; i < svm_optionCount; i++)
        {
            selectOption << svm_option[i] << endl;
        }

        cout << "////////////////////////////////////"<< endl;
        cout << selectOption.str();
        cout << "////////////////////////////////////"<< endl;
        cout << "请选择一项操作:";

        int select = -1;
        bool isRepeat = true;
        while (isRepeat)
        {
            cin >> select;
            isRepeat = false;
            switch (select)
            {
                case 1:
                    getLearnData();
                    break;
                case 2:
                    label_data();
                    break;
                case 3:
                    svmTrain(false, false);
                    break;
                case 4:
                    svmTrain(true, false);
                    break;
                case 5:
                    svmTrain(false, true);
                    break;
                case 6:
                    svmTrain();
                    break;
                case 7:
                    isExit = true;
                    break;
                default:
                    cout << "输入错误，请重新输入:";
                    isRepeat = true;
                    break;
            }
        }
    }
    return 0;
}

// SVM 训练命令行
////////////////////////////////////////////////////////////

// command line option support

// the function declaration to be used,
// owing to the modules of the current EasyPR is separated, this is only temporary.

int test_plate_locate();
int test_plate_judge();
int test_chars_segment();
int test_chars_identify();
int test_plate_detect();
int test_chars_recognise();
int test_plate_recognize();

void cmdMain(int argc, char *argv[])
{
    const char *help[] = {
        "EasyPR Usage:                                             ",
        "--help                 [ -h   ]    显示帮助                ",
        "测试模块                                                   ",
        "--test_plate_locate    [ -tpl ]    车牌定位                ",
        "--test_plate_judge     [ -tpj ]    车牌判断                ",
        "--test_plate_detect    [ -tpd ]    车牌检测                ",
        "--test_chars_segment   [ -tcs ]    字符分隔                ",
        "--test_chars_identify  [ -tci ]    字符鉴别                ",
        "--test_chars_recognize [ -tcr ]    字符识别                ",
        "--test_plate_recognize [ -tpr ]    车牌识别                ",
        "--test_all             [ -ta  ]    测试全部                ",
        "--general_test         [ -gt  ]    批量测试-general_test   ",
        "--native_test          [ -nt  ]    批量测试-native_test    ",
        "SVM训练",
        "--svm_gen_learndata    [ -sgl ]    生成Learndata           ",
        "--svm_tag_learndata    [ -stl ]    标签Learndata           ",
        "--svm_detect           [ -sd  ]    车牌检测，可设置-v或-t    ",
        "--svm_divide           [ -v   ]    启用分割                ",
        "--svm_train            [ -t   ]    启用训练                ",
        "ANN训练（未开放）                                           ",
        "GDTS生成                                                   ",
        "--gdts                 [ -gts  ]   GDTS生成                ",
        "--group                            开发团队                ",
        "--thanks                           感谢名单                ",
        "                                                          ",
        "Examples:                                                 ",
        "   $ ./EasyPR --test_plate_locate                         ",
        "Will do the same as followings:                           ",
        "   $ ./EasyPR -tpl                                        ",
        "   $ ./EasyPR -t -p -l                                    ",
        "   $ ./EasyPR -ptl                                        ",
        "   $ ./EasyPR -p -l -t                                    ",
        NULL
    };

    CParser parser(argc, argv);

    try {

        do {
            if (parser.has_or(2, "help", "h") /* || argc < 2 */) {
                int i = 0;
                while (help[i]) {
                    cout << help[i++] << endl;
                }
                break;
            }

            // tests

            if (parser.has_or(2, "test_plate_locate", "-tpl")) {
                cout << (test_plate_locate() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_plate_judge", "-tpj")) {
                cout << (test_plate_judge() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_plate_detect", "-tpd")) {
                cout << (test_plate_detect() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_chars_segment", "-tcs")) {
                cout << (test_chars_segment() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_chars_identify", "-tci")) {
                cout << (test_chars_identify() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_chars_recognize", "-tcr")) {
                cout << (test_chars_recognise() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_plate_recognize", "-tpr")) {
                cout << (test_plate_recognize() == 0 ? "passed" : "failed");
                break;
            }

            if (parser.has_or(2, "test_all", "-ta")) {
                cout << "test_plate_locate "    << (test_plate_locate()    == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_judge "     << (test_plate_judge()     == 0 ? "passed" : "failed") << endl;
                cout << "test_chars_segment "   << (test_chars_segment()   == 0 ? "passed" : "failed") << endl;
                cout << "test_chars_identify "  << (test_chars_identify()  == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_detect "    << (test_plate_detect()    == 0 ? "passed" : "failed") << endl;
                cout << "test_chars_recognize " << (test_chars_recognise() == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_recognize " << (test_plate_recognize() == 0 ? "passed" : "failed") << endl;
                cout << "test_plate_locate "    << (test_plate_locate()    == 0 ? "passed" : "failed") << endl;
                break;
            }

            // batch testing

            if (parser.has_or(2, "general_test", "-gt")) {
                acurayTest(GENERAL_TEST_PATH);
                break;
            }

            if (parser.has_or(2, "native_test", "-nt")) {
                acurayTest(NATIVE_TEST_PATH);
                break;
            }

            // svm trains

            if (parser.has_or(2, "svm_gen_learndata", "-sgl")) {
                getLearnData();
                break;
            }

            if (parser.has_or(2, "svm_tag_learndata", "-stl")) {
                label_data();
                break;
            }

            if (parser.has_or(2, "svm_detect", "-sd")) {
                svmTrain(parser.has_or(2, "v", "svm_divide"), parser.has_or(2, "t", "svm_train"));
                break;
            }

            // GDTS

            if (parser.has_or(2, "gdts", "gts")) {
                general_test();
                break;
            }

            //

            if (parser.has("group")) {
                // 开发团队;
                cout << endl;
                cout << "我们EasyPR团队目前有一个5人左右的小组在进行EasyPR后续版本的开发工作。" << endl;
                cout << "如果你对本项目感兴趣，并且愿意为开源贡献一份力量，我们很欢迎你的加入。" << endl;
                cout << "目前招聘的主要人才是：车牌定位，图像识别，深度学习，网站建设相关方面的牛人。" << endl;
                cout << "如果你觉得自己符合条件，请发邮件到地址(easypr_dev@163.com)，期待你的加入！" << endl;
                cout << endl;
                break;
            }

            if (parser.has("thanks")) {
                // 感谢名单
                cout << endl;
                cout << "本项目在建设过程中，受到了很多人的帮助，其中以下是对本项目做出突出贡献的" << endl;
                cout << "(贡献包括有益建议，代码调优，数据提供等等,排名按时间顺序)：" << endl;
                cout << "taotao1233，邱锦山，唐大侠，jsxyhelu，如果有一天(zhoushiwei)，学习奋斗，袁承志，圣城小石匠，goldriver，Micooz，梦里时光，Rain Wang，ahccoms，星夜落尘，海豚嘎嘎" << endl;
                cout << "还有很多的同学对本项目也给予了鼓励与支持，在此也一并表示真诚的谢意！" << endl;
                cout << endl;
                break;
            }

        } while(false);

    } catch (const std::exception &err) {
        cout << err.what() << endl;
    }
}
