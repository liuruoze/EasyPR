// ��������general_test�е�ͼƬ
// ͨ��������ʹ��easypr���ζ�ȡͼƬ���������е�һ������ʶ�����ָ�ͼƬ������
// easyprʶ������ֲ�һ��׼ȷ�������Ҫ���ٴ�ȷ�����ֹ�������

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/feature.h"

using namespace easypr;

extern const string GENERAL_TEST_PATH;

// TODO �������·���ĳ����
const string src_path = "F:/data/easypr-data/tmp-5";

// TODO �������·���ĳ����
const string dst_path = "F:/data/easypr-data/tmp-6";

int general_test()
{
    ////��ȡ��·���µ������ļ�
    auto files = Utils::getFiles(src_path);

    CPlateLocate lo;
    CPlateJudge ju;
    CPlateRecognize pr;

    pr.LoadANN("model/ann.xml");
    pr.LoadSVM("model/svm.xml");
    pr.setLifemode(true);

    int size = files.size();
    //int size = 200;

    if (0 == size)
    {
        cout << "No File Found!" << endl;
        return 0;
    }

    cout << "Begin to prepare general_test!" << endl;

    for (int i = 0; i < size; i++)
    {
        string filepath = files[i].c_str();
        cout << "------------------" << endl;

        // EasyPR��ʼ�жϳ���
        Mat src = imread(filepath);
        vector<string> plateVec;

        int result = pr.plateRecognize(src, plateVec);
        if (result == 0)
        {
            int num = plateVec.size();

            if (num == 0)
            {
                cout << ""<< "�޳���" <<endl;
            }
            else
            {
                cout << plateVec[0] <<endl;
                string colorplate = plateVec[0];

                // ���"����:��E7KU22"��ð�ź���ĳ���
                vector<string> spilt_plate = Utils::splitString(colorplate, ':');

                int size = spilt_plate.size();
                if (size == 2)
                {
                    stringstream ss(stringstream::in | stringstream::out);
                    ss << dst_path << "/" << spilt_plate[size-1] << ".jpg";
                    imwrite(ss.str(), src);
                }
            }
        }
        else
        {
            cout << "������:" << result << endl;
        }
    }

    return 0;
}
