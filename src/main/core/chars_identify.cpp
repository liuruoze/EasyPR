#include "../include/chars_identify.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

#define HORIZONTAL    1
#define VERTICAL    0
#define NDEBUG

//中国车牌
const char strCharacters[] = {'0','1','2','3','4','5',\
	'6','7','8','9','A','B', 'C', 'D', 'E','F', 'G', 'H', /* 没有I */\
	'J', 'K', 'L', 'M', 'N', /* 没有O */ 'P', 'Q', 'R', 'S', 'T', \
	'U','V', 'W', 'X', 'Y', 'Z'}; 
const int numCharacter = 34; /* 没有I和0,10个数字与24个英文字符之和 */

//以下都是我训练时用到的中文字符数据，并不全面，有些省份没有训练数据所以没有字符
const string strChinese[] = {"zh_cuan" /* 川 */, "zh_e" /* 鄂 */,  "zh_gan" /* 赣*/, \
	"zh_hei" /* 黑 */, "zh_hu" /* 沪 */,  "zh_ji" /* 冀 */, \
	"zh_jl" /* 吉 */, "zh_jin" /* 津 */, "zh_jing" /* 京 */, "zh_shan" /* 陕 */, \
	"zh_liao" /* 辽 */, "zh_lu" /* 鲁 */, "zh_min" /* 闽 */, "zh_ning" /* 宁 */, \
	"zh_su" /* 苏 */,  "zh_sx" /* 晋 */, "zh_wan" /* 皖 */,\
	 "zh_yu" /* 豫 */, "zh_yue" /* 粤 */, "zh_zhe" /* 浙 */};

const int numChinese = 20;
const int numAll = 54; /* 34+20=54 */

CCharsIdentify::CCharsIdentify()
{
	//cout << "CCharsIdentify" << endl;
	m_predictSize = 10;
	m_path = "model/ann.xml";
	LoadModel();

	if (m_map.empty())
	{
		m_map.insert(pair<string, string>("zh_cuan","川"));
		m_map.insert(pair<string, string>("zh_e","鄂"));
		m_map.insert(pair<string, string>("zh_gan","赣"));
		m_map.insert(pair<string, string>("zh_hei","黑"));
		m_map.insert(pair<string, string>("zh_hu","沪"));
		m_map.insert(pair<string, string>("zh_ji","冀"));
		m_map.insert(pair<string, string>("zh_jl","吉"));
		m_map.insert(pair<string, string>("zh_jin","津"));
		m_map.insert(pair<string, string>("zh_jing","京"));
		m_map.insert(pair<string, string>("zh_shan","陕"));
		m_map.insert(pair<string, string>("zh_liao","辽"));
		m_map.insert(pair<string, string>("zh_lu","鲁"));
		m_map.insert(pair<string, string>("zh_min","闽"));
		m_map.insert(pair<string, string>("zh_ning","宁"));
		m_map.insert(pair<string, string>("zh_su","苏"));
		m_map.insert(pair<string, string>("zh_sx","晋"));
		m_map.insert(pair<string, string>("zh_wan","皖"));
		m_map.insert(pair<string, string>("zh_yu","豫"));
		m_map.insert(pair<string, string>("zh_yue","粤"));
		m_map.insert(pair<string, string>("zh_zhe","浙"));
	}
}


void CCharsIdentify::LoadModel()
{
	ann.clear();
	ann.load(m_path.c_str(), "ann");
}

void CCharsIdentify::LoadModel(string s)
{
	ann.clear();
	ann.load(s.c_str(), "ann");
}

//create the accumulation histograms,img is a binary image, t is 水平或垂直
Mat CCharsIdentify::ProjectedHistogram(Mat img, int t)
{
	int sz=(t)?img.rows:img.cols;
	Mat mhist=Mat::zeros(1,sz,CV_32F);

	for(int j=0; j<sz; j++){
		Mat data=(t)?img.row(j):img.col(j);
		mhist.at<float>(j)=countNonZero(data);	//统计这一行或一列中，非零元素的个数，并保存到mhist中
	}

	//Normalize histogram
	double min, max;
	minMaxLoc(mhist, &min, &max);

	if(max>0)
		mhist.convertTo(mhist,-1 , 1.0f/max, 0);//用mhist直方图中的最大值，归一化直方图

	return mhist;
}

//! 获得字符的特征图
Mat CCharsIdentify::features(Mat in, int sizeData)
{
	//Histogram features
	Mat vhist=ProjectedHistogram(in, VERTICAL);
	Mat hhist=ProjectedHistogram(in, HORIZONTAL);

	//Low data feature
	Mat lowData;
	resize(in, lowData, Size(sizeData, sizeData) );

	//Last 10 is the number of moments components
	int numCols = vhist.cols + hhist.cols + lowData.cols*lowData.cols;

	Mat out = Mat::zeros(1, numCols, CV_32F);

	//Asign values to feature,ANN的样本特征为水平、垂直直方图和低分辨率图像所组成的矢量
	int j=0;
	for(int i=0; i<vhist.cols; i++)
	{
		out.at<float>(j)=vhist.at<float>(i);
		j++;
	}
	for(int i=0; i<hhist.cols; i++)
	{
		out.at<float>(j)=hhist.at<float>(i);
		j++;
	}
	for(int x=0; x<lowData.cols; x++)
	{
		for(int y=0; y<lowData.rows; y++){
			out.at<float>(j)=(float)lowData.at<unsigned char>(x,y);
			j++;
		}
	}

	return out;
}


//! 利用神经网络做识别
int CCharsIdentify::classify(Mat f, bool isChinses){
	int result = -1;
	Mat output(1, numAll, CV_32FC1);
	ann.predict(f, output);

	if (!isChinses)
	{
		result = 0;
		float maxVal = -2;
		for(int j = 0; j < numCharacter; j++)
		{
			float val = output.at<float>(j);
			//cout << "j:" << j << "val:"<< val << endl;
			if (val > maxVal)
			{
				maxVal = val;
				result = j;
			}
		}
	}
	else
	{
		result = numCharacter;
		float maxVal = -2;
		for(int j = numCharacter; j < numAll; j++)
		{
			float val = output.at<float>(j);
			//cout << "j:" << j << "val:"<< val << endl;
			if (val > maxVal)
			{
				maxVal = val;
				result = j;
			}
		}
	}
	return result;
}


//输入当个字符Mat,生成字符的string
string CCharsIdentify::charsIdentify(Mat input, bool isChinese)
{
	Mat f = features(input, m_predictSize);
	string result = "";
	int index = classify(f, isChinese);

	if (!isChinese)
	{
		result = result + strCharacters[index];
	}
	else
	{
		string s = strChinese[index - numCharacter];
		string province = m_map[s];
		result = province + result;
	}

	return result;
}



}	/*! \namespace easypr*/
