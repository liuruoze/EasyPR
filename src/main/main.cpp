#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace easypr;

int svmMain();

namespace easypr {

	int svmTrain(bool dividePrepared = true, bool trainPrepared = true,
	svmCallback getFeatures = getHistogramFeatures);

}


////////////////////////////////////////////////////////////
// EasyPR 训练命令行

const string option[] = 
	{
		"1. 测试;"	,
		"2. 批量测试(未开放);"		,
		"3. SVM训练;"		,
		"4. ANN训练(未开放);"		,
		"5. 开发团队;"		,
		"6. 感谢名单;"		,
		"7. 退出;"			,  
	};

const int optionCount = 7;

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
				// TODO
				break;
			case 3:
				svmMain();
				break;
			case 4:
				// TODO
				break;
			case 5:
				// TODO;
				break;
			case 6:
				// 感谢名单
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
// /EasyPR 训练命令行 结束
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// SVM 训练命令行 

const string svm_option[] = 
	{
		"1. 生成learndata;"	,
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
		selectOption << "svmMain Option:"<< endl;
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