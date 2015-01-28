#include "../include/chars_recognise.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

CCharsRecognise::CCharsRecognise()
{
	//cout << "CCharsRecognise" << endl;
	m_charsSegment = new CCharsSegment();
	m_charsIdentify = new CCharsIdentify();
}

void CCharsRecognise::LoadANN(string s)
{
	m_charsIdentify->LoadModel(s.c_str());
}

int CCharsRecognise::charsRecognise(Mat plate, string& plateLicense)
{
	//车牌字符方块集合
	vector<Mat> matVec;

	string plateIdentify = "";

	int result = m_charsSegment->charsSegment(plate, matVec);
	if (result == 0)
	{
		int num = matVec.size();
		for (int j = 0; j < num; j++)
		{
			Mat charMat = matVec[j];
			bool isChinses = false;

			//默认首个字符块是中文字符
			if (j == 0)
				isChinses = true;

			string charcater = m_charsIdentify->charsIdentify(charMat, isChinses);
			plateIdentify = plateIdentify + charcater;
		}
	}

	plateLicense = plateIdentify;

	return 0;
}

}	/*! \namespace easypr*/