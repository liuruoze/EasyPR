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
// EasyPR ѵ��������

const string option[] =
{
    "1. ����;"	,
    "2. ��������;"		,
    "3. SVMѵ��;"		,
    "4. ANNѵ��(δ����);"		,
    "5. GDTS����;"		,
    "6. �����Ŷ�;"		,
    "7. ��л����;"		,
    "8. �˳�;"			,
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
        cout << "��ѡ��һ�����:";

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
                    // �����Ŷ�;
                    cout << endl;
                    cout << "����EasyPR�Ŷ�Ŀǰ��һ��5�����ҵ�С���ڽ���EasyPR�����汾�Ŀ���������" << endl;
                    cout << "�����Ա���Ŀ����Ȥ������Ը��Ϊ��Դ����һ�����������Ǻܻ�ӭ��ļ��롣" << endl;
                    cout << "Ŀǰ��Ƹ����Ҫ�˲��ǣ����ƶ�λ��ͼ��ʶ�����ѧϰ����վ������ط����ţ�ˡ�" << endl;
                    cout << "���������Լ������������뷢�ʼ�����ַ(easypr_dev@163.com)���ڴ���ļ��룡" << endl;
                    cout << endl;
                    break;
                case 7:
                    // ��л����
                    cout << endl;
                    cout << "����Ŀ�ڽ�������У��ܵ��˺ܶ��˵İ��������������ǶԱ���Ŀ����ͻ�����׵�" << endl;
                    cout << "(���װ������潨�飬������ţ������ṩ�ȵ�,������ʱ��˳��)��" << endl;
                    cout << "taotao1233���ƴ�����jsxyhelu�������һ��(zhoushiwei)��ѧϰ�ܶ���Ԭ��־��ʥ��Сʯ����goldriver��Micooz������ʱ�⣬Rain Wang��ahccom" << endl;
                    cout << "���кܶ��ͬѧ�Ա���ĿҲ�����˹�����֧�֣��ڴ�Ҳһ����ʾ��ϵ�л�⣡" << endl;
                    cout << endl;
                    break;
                case 8:
                    isExit = true;
                    break;
                default:
                    cout << "�����������������:";
                    isRepeat = true;
                    break;
            }
        }
    }
    return 0;
}
// /EasyPR ѵ�������� ����
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// acurayTestMain ������

const string acuray_option[] =
{
    "1. general_test;"	,
    "2. native_test;"	,
    "3. ����;"			,
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
        cout << "��ѡ��һ�����:";

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
                    cout << "�����������������:";
                    isRepeat = true;
                    break;
            }
        }
    }
    return 0;
}

// acurayTestMain ������
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// SVM ѵ��������

const string svm_option[] =
{
    "1. ����learndata(�������뵽��Ļ���������);"	,
    "2. ��ǩlearndata;"		,
    "3. ���Ƽ��(not divide and train);"		,
    "4. ���Ƽ��(not train);"		,
    "5. ���Ƽ��(not divide);"		,
    "6. ���Ƽ��;"		,
    "7. ����;"			,
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
        cout << "��ѡ��һ�����:";

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
                    cout << "�����������������:";
                    isRepeat = true;
                    break;
            }
        }
    }
    return 0;
}

// SVM ѵ��������
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
        "--help                 [ -h   ]    ��ʾ����                ",
        "����ģ��                                                   ",
        "--test_plate_locate    [ -tpl ]    ���ƶ�λ                ",
        "--test_plate_judge     [ -tpj ]    �����ж�                ",
        "--test_plate_detect    [ -tpd ]    ���Ƽ��                ",
        "--test_chars_segment   [ -tcs ]    �ַ��ָ�                ",
        "--test_chars_identify  [ -tci ]    �ַ�����                ",
        "--test_chars_recognize [ -tcr ]    �ַ�ʶ��                ",
        "--test_plate_recognize [ -tpr ]    ����ʶ��                ",
        "--test_all             [ -ta  ]    ����ȫ��                ",
        "--general_test         [ -gt  ]    ��������-general_test   ",
        "--native_test          [ -nt  ]    ��������-native_test    ",
        "SVMѵ��",
        "--svm_gen_learndata    [ -sgl ]    ����Learndata           ",
        "--svm_tag_learndata    [ -stl ]    ��ǩLearndata           ",
        "--svm_detect           [ -sd  ]    ���Ƽ�⣬������-v��-t    ",
        "--svm_divide           [ -v   ]    ���÷ָ�                ",
        "--svm_train            [ -t   ]    ����ѵ��                ",
        "ANNѵ����δ���ţ�                                           ",
        "GDTS����                                                   ",
        "--gdts                 [ -gts  ]   GDTS����                ",
        "--group                            �����Ŷ�                ",
        "--thanks                           ��л����                ",
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
                // �����Ŷ�;
                cout << endl;
                cout << "����EasyPR�Ŷ�Ŀǰ��һ��5�����ҵ�С���ڽ���EasyPR�����汾�Ŀ���������" << endl;
                cout << "�����Ա���Ŀ����Ȥ������Ը��Ϊ��Դ����һ�����������Ǻܻ�ӭ��ļ��롣" << endl;
                cout << "Ŀǰ��Ƹ����Ҫ�˲��ǣ����ƶ�λ��ͼ��ʶ�����ѧϰ����վ������ط����ţ�ˡ�" << endl;
                cout << "���������Լ������������뷢�ʼ�����ַ(easypr_dev@163.com)���ڴ���ļ��룡" << endl;
                cout << endl;
                break;
            }

            if (parser.has("thanks")) {
                // ��л����
                cout << endl;
                cout << "����Ŀ�ڽ�������У��ܵ��˺ܶ��˵İ��������������ǶԱ���Ŀ����ͻ�����׵�" << endl;
                cout << "(���װ������潨�飬������ţ������ṩ�ȵ�,������ʱ��˳��)��" << endl;
                cout << "taotao1233���ƴ�����jsxyhelu�������һ��(zhoushiwei)��ѧϰ�ܶ���Ԭ��־��ʥ��Сʯ����goldriver��Micooz������ʱ�⣬Rain Wang��" << endl;
                cout << "���кܶ��ͬѧ�Ա���ĿҲ�����˹�����֧�֣��ڴ�Ҳһ����ʾ��ϵ�л�⣡" << endl;
                cout << endl;
                break;
            }

        } while(false);

    } catch (const std::exception &err) {
        cout << err.what() << endl;
    }
}
