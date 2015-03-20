#include "../include/plate_detect.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

CPlateDetect::CPlateDetect()
{
	//cout << "CPlateDetect" << endl;
	m_plateLocate = new CPlateLocate();
	m_plateJudge = new CPlateJudge();

	// 默认EasyPR在一幅图中定位最多3个车
	m_maxPlates = 3;
}

void CPlateDetect::LoadSVM(string s)
{
	m_plateJudge->LoadModel(s.c_str());
}

int CPlateDetect::plateDetect(Mat src, vector<Mat>& resultVec, int index)
{
	//可能是车牌的图块集合
	vector<Mat> matVec;

	int resultLo = m_plateLocate->plateLocate(src, matVec);

	if (0 != resultLo)
		return -1;

	int resultJu = m_plateJudge->plateJudge(matVec, resultVec);
		
	if(getPDDebug())
	{ 
		int size = resultVec.size();
		for (int i = 0; i < size; i++)
		{
			Mat img = resultVec[i];
			if(1)
			{
				stringstream ss(stringstream::in | stringstream::out);
				ss << "image/tmp/plate_judge_result_" << i << ".jpg";
				imwrite(ss.str(), img);
			}
		}
	}	


	if (0 != resultJu)
		return -2;

	return 0;
}


int CPlateDetect::plateDetectDeep(Mat src, vector<Mat>& resultVec, bool showDetectArea, int index)
{

	vector<Mat> resultPlates;

	vector<CPlate> color_Plates;
	vector<CPlate> sobel_Plates;
	vector<CPlate> color_result_Plates;
	vector<CPlate> sobel_result_Plates;

	vector<CPlate> all_result_Plates;

	//如果颜色查找找到n个以上（包含n个）的车牌，就不再进行Sobel查找了。
	const int color_find_max = m_maxPlates;

	Mat result;
	src.copyTo(result);

	m_plateLocate->plateColorLocate(src, color_Plates, index);
	m_plateJudge->plateJudge(color_Plates, color_result_Plates);

	
	for (int i = 0; i< color_result_Plates.size(); i++)
	{
		CPlate plate = color_result_Plates[i];

		RotatedRect minRect = plate.getPlatePos();
		Point2f rect_points[4];
		minRect.points(rect_points);
		for (int j = 0; j < 4; j++)
			line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 255), 2, 8);

		all_result_Plates.push_back(plate);
	}

	if (color_result_Plates.size() >= color_find_max)
	{
		//如果颜色查找找到n个以上（包含n个）的车牌，就不再进行Sobel查找了。
	} 
	else
	{
		m_plateLocate->plateSobelLocate(src, sobel_Plates, index);
		m_plateJudge->plateJudge(sobel_Plates, sobel_result_Plates);

		for (int i = 0; i< sobel_result_Plates.size(); i++)
		{
			CPlate plate = sobel_result_Plates[i];

			RotatedRect minRect = plate.getPlatePos();
			Point2f rect_points[4]; 
			minRect.points( rect_points );
			for( int j = 0; j < 4; j++ )
				line(result, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 2, 8 );

			all_result_Plates.push_back(plate);
		}
	}

	for (int i = 0; i < all_result_Plates.size(); i++)
	{
		// 把截取的车牌图像依次放到左上角
		CPlate plate = all_result_Plates[i];

		Mat plate_area = plate.getPlateMat();

		int height = m_plateLocate->HEIGHT;
		int width = m_plateLocate->WIDTH;

		assert(height*i + height < result.rows);

		Mat imageRoi = result(Rect(0, 0 + height*i, width, height));
		addWeighted(imageRoi, 0, plate_area, 1, 0, imageRoi);

		resultVec.push_back(plate_area);
	}

	if (showDetectArea)
	{
		namedWindow("EasyPR", CV_WINDOW_AUTOSIZE);
		showResult(result);
		destroyWindow("EasyPR");
	}
	

	if (0)
	{
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/" << index << "_" << 9 <<"_result" << ".jpg";
		imwrite(ss.str(), result);
	}

	return 0;
}

int CPlateDetect::showResult(const Mat& result)
{
	const int RESULTWIDTH = 640;  //640 930
	const int RESULTHEIGHT = 540;   //540 710

	Mat img_window;
	img_window.create(RESULTHEIGHT, RESULTWIDTH, CV_8UC3);

	int nRows = result.rows;
	int nCols = result.cols;

	Mat result_resize;
	if (nCols <= img_window.cols && nRows <= img_window.rows) {
		result_resize = result;

	} else if (nCols > img_window.cols && nRows <= img_window.rows) {
		float scale = float(img_window.cols) / float(nCols);
		resize(result, result_resize, Size(), scale, scale, CV_INTER_AREA);

	} else if (nCols <= img_window.cols && nRows > img_window.rows) {
		float scale = float(img_window.rows) / float(nRows);
		resize(result, result_resize, Size(), scale, scale, CV_INTER_AREA);

	} else if (nCols > img_window.cols && nRows > img_window.rows) {
		Mat result_middle;
		float scale = float(img_window.cols) / float(nCols);
		resize(result, result_middle, Size(), scale, scale, CV_INTER_AREA);

		if (result_middle.rows > img_window.rows) {
			float scale = float(img_window.rows) / float(result_middle.rows);
			resize(result_middle, result_resize, Size(), scale, scale, CV_INTER_AREA);

		}
		else {
			result_resize = result_middle;
		}
	} else {
		result_resize = result;
	}

	Mat imageRoi = img_window(Rect((RESULTWIDTH - result_resize.cols) / 2, (RESULTHEIGHT - result_resize.rows) / 2,
		result_resize.cols, result_resize.rows));
	addWeighted(imageRoi, 0, result_resize, 1, 0, imageRoi);

	
	imshow("EasyPR", img_window);
	waitKey(0);

	return 0;
}



}	/*! \namespace easypr*/