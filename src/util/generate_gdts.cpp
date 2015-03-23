//	����GDTS���ļ�
//	�ھ������ݵ�GDTS���ݼ������������ķ�����ͼ�����Ԥ����
//	EasyPR��Դ��Ŀ�ǳ�ע�ر���ԭʼͼƬ�İ�Ȩ
//  ���еľ�������ͨ��GDSLЭ�����Լ������֤ʹ���˽����ڷ���ҵ��Ŀ��

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/feature.h"

using namespace easypr;

// TODO �������·���ĳ����ԭʼͼƬ·��
// ͼƬ��Ҫ�࣬10-30�ž��㹻�ˣ�EasyPR��GDTS���ݼ���ʹ�ò�����Ϊ��Ҫָ��
// ֻҪ��ЩͼƬ�㹻��ӳ�����ݼ�����Ҫ��������
const string src_path = "F:/data/easypr-data/tmp-1";

// TODO �������·���ĳ���ϣ�����ɾ�����GDTS���ݴ�ŵ���·��
const string dst_path = "F:/data/easypr-data/tmp-2";

// EasyPR��ͼ��Ԥ������������ģ������ü�������
Mat imageProcess(Mat src);

Mat detectAndMaskFace(Mat& img, CascadeClassifier& cascade,
                       double scale );

int generate_gdts()
{
	// ��ȡ����ʶ���ļ�
	CascadeClassifier cascade;
    //string cascadeName="/Users/zhoushiwei/Dropbox/Avatar/SDMLIB/haarcascade_frontalface_alt2.xml";
	string cascadeName="model/haarcascade_frontalface_alt_tree.xml";

	////��ȡ��·���µ������ļ�
        auto files = Utils::getFiles(src_path);
	int size = files.size();

	if (0 == size)
	{
		cout << "No File Found!" << endl;
		return 0;
	}

	cout << "Begin to prepare generate_gdts!" << endl;

	for (int i = 0; i < size; i++)
	{
		string filepath = files[i].c_str();
		cout << "------------------" << endl;
		cout << filepath << endl;

		// EasyPR��ȡԭͼƬ
		Mat src = imread(filepath);

		// EasyPR��ʼ��ͼƬ����ģ������ü�������
		Mat img = imageProcess(src);

		// EasyPR��ʼ��ͼƬ��������ʶ����
		Mat dst = detectAndMaskFace(img, cascade, 1.5);

		// ��ͼƬ��������·��
        vector<string> spilt_path = Utils::splitString(filepath, '\\');

		int spiltsize = spilt_path.size();
		string filename = "";

		if (spiltsize != 0)
			filename = spilt_path[spiltsize-1];

		stringstream ss(stringstream::in | stringstream::out);
		ss << dst_path << "/" << filename;
		imwrite(ss.str(), dst);
	}

	return 0;
}




































// EasyPR��ͼ��Ԥ������������ģ������ü�������
Mat imageProcess(Mat img)
{
	int width = img.size().width;
	int height = img.size().height;
	Rect rect(width*0.01, height*0.01, width*0.99, height*0.99);

	Mat dst = img(rect);
	//GaussianBlur( dst, dst, Size(1, 1), 0, 0, BORDER_DEFAULT );
	return dst;
}
