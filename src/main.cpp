#include "include/plate_recognize.h"
#include "include/util.h"
#include "include/features.h"

using namespace easypr;

int svmMain();
int acurayTestMain();

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

int main()
{
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
				cout << "taotao1233, 唐大侠，jsxyhelu，如果有一天，学习奋斗，袁承志，圣城小石匠，" << endl;
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