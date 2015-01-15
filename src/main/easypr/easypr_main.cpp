#include "../include/plate_recognize.h"

using namespace easypr;

/// test.cpp中方法
int testMain();

/// data_prepare.cpp中方法
void getLearnData();

const string option[] = 
	{
		"1. 生成learndata;"	,
		"2. 测试EasyPR;"		,  
		"3. 退出;"			,  
	};

const int optionCount = 3;

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
				getLearnData();
				break;
			case 2:
				testMain();
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
