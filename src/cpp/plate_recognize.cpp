#include "../include/plate_recognize.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

CPlateRecognize::CPlateRecognize()
{
	cout << "CPlateRecognize" << endl;
	m_plateDetect= new CPlateDetect();
	m_charsRecognise = new CCharsRecognise();
}

int CPlateRecognize::plateRecognize(Mat src, vector<string>& plateVec)
{
	//车牌方块集合
	vector<Mat> matVec;

	int resultPD = m_plateDetect->plateDetect(src, matVec);
	if (resultPD == 0)
	{
		int num = matVec.size();
		for (int j = 0; j < num; j++)
		{
			string plateIdentify = "";
			int resultCR = m_charsRecognise->charsRecognise(matVec[j], plateIdentify);
			if (resultCR == 0)
			{
				plateVec.push_back(plateIdentify);
			}
		}
	}

	return resultPD;
}

}	/*! \namespace easypr*/

