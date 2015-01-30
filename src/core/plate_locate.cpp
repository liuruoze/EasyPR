#include "../include/plate_locate.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr{

const float DEFAULT_ERROR = 0.6;
const float DEFAULT_ASPECT = 3.75; 

CPlateLocate::CPlateLocate()
{
	//cout << "CPlateLocate" << endl;
	m_GaussianBlurSize = DEFAULT_GAUSSIANBLUR_SIZE;
	m_MorphSizeWidth = DEFAULT_MORPH_SIZE_WIDTH;
	m_MorphSizeHeight = DEFAULT_MORPH_SIZE_HEIGHT;

	m_error = DEFAULT_ERROR;
	m_aspect = DEFAULT_ASPECT;
	m_verifyMin = DEFAULT_VERIFY_MIN;
	m_verifyMax = DEFAULT_VERIFY_MAX;

	m_angle = DEFAULT_ANGLE;

	m_debug = DEFAULT_DEBUG;
}

//! 生活模式与工业模式切换
//! 如果为真，则设置各项参数为定位生活场景照片（如百度图片）的参数，否则恢复默认值。
void CPlateLocate::setLifemode(bool param)
{
	if(param == true)
	{
		setGaussianBlurSize(5);
		setMorphSizeWidth(9);
		setMorphSizeHeight(3);
		setVerifyError(0.9);
		setVerifyAspect(4);
		setVerifyMin(1);
		setVerifyMax(30);
	} 
	else
	{
		setGaussianBlurSize(DEFAULT_GAUSSIANBLUR_SIZE);
		setMorphSizeWidth(DEFAULT_MORPH_SIZE_WIDTH);
		setMorphSizeHeight(DEFAULT_MORPH_SIZE_HEIGHT);
		setVerifyError(DEFAULT_ERROR);
		setVerifyAspect(DEFAULT_ASPECT);
		setVerifyMin(DEFAULT_VERIFY_MIN);
		setVerifyMax(DEFAULT_VERIFY_MAX);
	}
}


//! 对minAreaRect获得的最小外接矩形，用纵横比进行判断
bool CPlateLocate::verifySizes(RotatedRect mr)
{
	float error = m_error;
	//Spain car plate size: 52x11 aspect 4,7272
	//China car plate size: 440mm*140mm，aspect 3.142857
	float aspect = m_aspect;
	//Set a min and max area. All other patchs are discarded
	//int min= 1*aspect*1; // minimum area
	//int max= 2000*aspect*2000; // maximum area
	int min= 44*14*m_verifyMin; // minimum area
	int max= 44*14*m_verifyMax; // maximum area
	//Get only patchs that match to a respect ratio.
	float rmin= aspect-aspect*error;
	float rmax= aspect+aspect*error;

	int area= mr.size.height * mr.size.width;
	float r = (float)mr.size.width / (float)mr.size.height;
	if(r < 1)
	{
		r= (float)mr.size.height / (float)mr.size.width;
	}

	if(( area < min || area > max ) || ( r < rmin || r > rmax ))
	{
		return false;
	}
	else
	{
		return true;
	}
}

//! 显示最终生成的车牌图像，便于判断是否成功进行了旋转。
Mat CPlateLocate::showResultMat(Mat src, Size rect_size, Point2f center, int index)
{
	Mat img_crop;
	getRectSubPix(src, rect_size, center, img_crop);

	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_crop_" << index << ".jpg";
		imwrite(ss.str(), img_crop);
	}

	Mat resultResized;
	resultResized.create(HEIGHT, WIDTH, TYPE);

	resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_resize_" << index << ".jpg";
		imwrite(ss.str(), resultResized);
	}

	return resultResized;
}

//! 定位车牌图像
//! src 原始图像
//! resultVec 一个Mat的向量，存储所有抓取到的图像
//! 成功返回0，否则返回-1
int CPlateLocate::plateLocate(Mat src, vector<Mat>& resultVec)
{
	Mat src_blur, src_gray;
	Mat grad;

	int scale = SOBEL_SCALE;
	int delta = SOBEL_DELTA;
	int ddepth = SOBEL_DDEPTH;

	if( !src.data )
	{ return -1; }

	//高斯模糊。Size中的数字影响车牌定位的效果。
	GaussianBlur( src, src_blur, Size(m_GaussianBlurSize, m_GaussianBlurSize), 
		0, 0, BORDER_DEFAULT );

	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_GaussianBlur" << ".jpg";
		imwrite(ss.str(), src_blur);
	}

	/// Convert it to gray
	cvtColor( src_blur, src_gray, CV_RGB2GRAY );

	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_gray" << ".jpg";
		imwrite(ss.str(), src_gray);
	}

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );

	/// Total Gradient (approximate)
	addWeighted( abs_grad_x, SOBEL_X_WEIGHT, abs_grad_y, SOBEL_Y_WEIGHT, 0, grad );

	//Laplacian( src_gray, grad_x, ddepth, 3, scale, delta, BORDER_DEFAULT );  
	//convertScaleAbs( grad_x, grad );  


	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_Sobel" << ".jpg";
		imwrite(ss.str(), grad);
	}

	Mat img_threshold;
	threshold(grad, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
	//threshold(grad, img_threshold, 75, 255, CV_THRESH_BINARY);

	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_threshold" << ".jpg";
		imwrite(ss.str(), img_threshold);
	}

	Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight) );
	morphologyEx(img_threshold, img_threshold, MORPH_CLOSE, element);
	
	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_morphology" << ".jpg";
		imwrite(ss.str(), img_threshold);
	}

	//Find 轮廓 of possibles plates
	vector< vector< Point> > contours;
	findContours(img_threshold,
		contours, // a vector of contours
		CV_RETR_EXTERNAL, // 提取外部轮廓
		CV_CHAIN_APPROX_NONE); // all pixels of each contours

	Mat result;
	if(m_debug)
	{ 
		//// Draw blue contours on a white image
		src.copyTo(result);
		drawContours(result, contours,
			-1, // draw all contours
			Scalar(0,0,255), // in blue
			1); // with a thickness of 1
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_Contours" << ".jpg";
		imwrite(ss.str(), result);
	}


	//Start to iterate to each contour founded
	vector<vector<Point> >::iterator itc = contours.begin();
	
	vector<RotatedRect> rects;
	//Remove patch that are no inside limits of aspect ratio and area.
	int t = 0;
	while (itc != contours.end())
	{
		//Create bounding rect of object
		RotatedRect mr = minAreaRect(Mat(*itc));

		//large the rect for more
		if( !verifySizes(mr))
		{
			itc = contours.erase(itc);
		}
		else
		{
			++itc;
			rects.push_back(mr);
		}
	}

	int k = 1;
	for(int i=0; i< rects.size(); i++)
	{
		RotatedRect minRect = rects[i];
		if(verifySizes(minRect))
		{	
			// rotated rectangle drawing 
			// Get rotation matrix
			// 旋转这部分代码确实可以将某些倾斜的车牌调整正，
			// 但是它也会误将更多正的车牌搞成倾斜！所以综合考虑，还是不使用这段代码。
			// 2014-08-14,由于新到的一批图片中发现有很多车牌是倾斜的，因此决定再次尝试
			// 这段代码。
			if(m_debug)
			{ 
				Point2f rect_points[4]; 
				minRect.points( rect_points );
				for( int j = 0; j < 4; j++ )
					line( result, rect_points[j], rect_points[(j+1)%4], Scalar(0,255,255), 1, 8 );
			}

			float r = (float)minRect.size.width / (float)minRect.size.height;
			float angle = minRect.angle;
			Size rect_size = minRect.size;
			if (r < 1)
			{
				angle = 90 + angle;
				swap(rect_size.width, rect_size.height);
			}
			//如果抓取的方块旋转超过m_angle角度，则不是车牌，放弃处理
			if (angle - m_angle < 0 && angle + m_angle > 0)
			{
				//Create and rotate image
				Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
				Mat img_rotated;
				warpAffine(src, img_rotated, rotmat, src.size(), CV_INTER_CUBIC);

				Mat resultMat;
				resultMat = showResultMat(img_rotated, rect_size, minRect.center, k++);

				resultVec.push_back(resultMat);
			}
		}
	}

	if(m_debug)
	{ 
		stringstream ss(stringstream::in | stringstream::out);
		ss << "image/tmp/debug_result" << ".jpg";
		imwrite(ss.str(), result);
	}

	return 0;
}

}	/*! \namespace easypr*/