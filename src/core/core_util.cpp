// 这个文件定义了EasyPR里所有plate判断的通用函数
// 所属命名空间为easypr
// 这个部分中的函数轻易不要改动

#include "../include/prep.h"
#include "../include/core_util.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

	//! 根据一幅图像与颜色模板获取对应的二值图
	//! 输入RGB图像, 颜色模板（蓝色、黄色）
	//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）
	Mat colorMatch(const Mat& src, Mat& match, const Color r)
	{
		// S和V的最小值由adaptive_minsv这个bool值判断
		// 如果为true，则最小值取决于H值，按比例衰减
		// 如果为false，则不再自适应，使用固定的最小值minabs_sv
		// 默认为false
		const int max_sv = 255;
		const int minref_sv = 64;
		const int minabs_sv = 90;

		const bool adaptive_minsv = false;

		//blue的H范围
		const int min_blue = 100;
		const int max_blue = 140;

		//yellow的H范围
		const int min_yellow = 15;
		const int max_yellow = 40;

		Mat src_hsv;
		// 转到HSV空间进行处理，颜色搜索主要使用的是H分量进行蓝色与黄色的匹配工作
		cvtColor(src, src_hsv, CV_BGR2HSV);

		vector<Mat> hsvSplit;
		split(src_hsv, hsvSplit);
		equalizeHist(hsvSplit[2], hsvSplit[2]);
		merge(hsvSplit, src_hsv);

		//匹配模板基色,切换以查找想要的基色
		int min_h = 0;
		int max_h = 0;
		switch (r) {
		case BLUE:
			min_h = min_blue;
			max_h = max_blue;
			break;
		case YELLOW:
			min_h = min_yellow;
			max_h = max_yellow;
			break;
		}

		float diff_h = float((max_h - min_h) / 2);
		int avg_h = min_h + diff_h;

		int channels = src_hsv.channels();
		int nRows = src_hsv.rows;
		//图像数据列需要考虑通道数的影响；
		int nCols = src_hsv.cols * channels;

		if (src_hsv.isContinuous())//连续存储的数据，按一行处理
		{
			nCols *= nRows;
			nRows = 1;
		}

		int i, j;
		uchar* p;
		float s_all = 0;
		float v_all = 0;
		float count = 0;
		for (i = 0; i < nRows; ++i)
		{
			p = src_hsv.ptr<uchar>(i);
			for (j = 0; j < nCols; j += 3)
			{
				int H = int(p[j]); //0-180
				int S = int(p[j + 1]);  //0-255
				int V = int(p[j + 2]);  //0-255

				s_all += S;
				v_all += V;
				count++;

				bool colorMatched = false;

				if (H > min_h && H < max_h)
				{
					int Hdiff = 0;
					if (H > avg_h)
						Hdiff = H - avg_h;
					else
						Hdiff = avg_h - H;

					float Hdiff_p = float(Hdiff) / diff_h;

					// S和V的最小值由adaptive_minsv这个bool值判断
					// 如果为true，则最小值取决于H值，按比例衰减
					// 如果为false，则不再自适应，使用固定的最小值minabs_sv
					int min_sv = 0;
					if (true == adaptive_minsv)
						min_sv = minref_sv - minref_sv / 2 * (1 - Hdiff_p);
					else
						min_sv = minabs_sv; // add

					if ((S > min_sv && S < max_sv) && (V > min_sv && V < max_sv))
						colorMatched = true;
				}

				if (colorMatched == true) {
					p[j] = 0; p[j + 1] = 0; p[j + 2] = 255;
				}
				else {
					p[j] = 0; p[j + 1] = 0; p[j + 2] = 0;
				}
			}
		}

		//cout << "avg_s:" << s_all / count << endl;
		//cout << "avg_v:" << v_all / count << endl;

		// 获取颜色匹配后的二值灰度图
		Mat src_grey;
		vector<Mat> hsvSplit_done;
		split(src_hsv, hsvSplit_done);
		src_grey = hsvSplit_done[2];

		match = src_grey;

		return src_grey;
	}


	//! 判断一个车牌的颜色
	//! 输入车牌mat与颜色模板
	//! 返回true或fasle
	bool plateColorJudge(Mat src, const Color r)
	{
		// 判断阈值
		const float thresh = 0.5;

		Mat src_gray;
		colorMatch(src, src_gray, r);

		float percent = float(countNonZero(src_gray)) / float(src_gray.rows * src_gray.cols);
		//cout << "percent:" << percent << endl;

		if (percent > thresh)
			return true;
		else
			return false;
	}

	//getPlateType
	//判断车牌的类型
	Color getPlateType(Mat src)
	{
		if (plateColorJudge(src, BLUE) == true) {
			//cout << "BLUE" << endl;
			return BLUE;
		}
		else if (plateColorJudge(src, YELLOW) == true) {
			//cout << "YELLOW" << endl;
			return YELLOW;
		}
		else {
			//cout << "OTHER" << endl;
			return BLUE;
		}
	}

	//clearLiuDing
	//去除车牌上方的钮钉
	//计算每行元素的阶跃数，如果小于X认为是柳丁，将此行全部填0（涂黑）
	//X的推荐值为，可根据实际调整
	Mat clearLiuDing(Mat img)
	{
		const int x = 7;
		Mat jump = Mat::zeros(1, img.rows, CV_32F);
		for (int i = 0; i < img.rows; i++)
		{
			int jumpCount = 0;
			for (int j = 0; j < img.cols - 1; j++)
			{
				if (img.at<char>(i, j) != img.at<char>(i, j + 1))
					jumpCount++;
			}
			jump.at<float>(i) = jumpCount;
		}
		for (int i = 0; i < img.rows; i++)
		{
			if (jump.at<float>(i) <= x)
			{
				for (int j = 0; j < img.cols; j++)
				{
					img.at<char>(i, j) = 0;
				}
			}
		}
		return img;
	}


	//! 直方图均衡
	Mat histeq(Mat in)
	{
		Mat out(in.size(), in.type());
		if (in.channels() == 3)
		{
			Mat hsv;
			vector<Mat> hsvSplit;
			cvtColor(in, hsv, CV_BGR2HSV);
			split(hsv, hsvSplit);
			equalizeHist(hsvSplit[2], hsvSplit[2]);
			merge(hsvSplit, hsv);
			cvtColor(hsv, out, CV_HSV2BGR);
		}
		else if (in.channels() == 1)
		{
			equalizeHist(in, out);
		}
		return out;
	}


	// ！获取垂直和水平方向直方图
	Mat ProjectedHistogram(Mat img, int t)
	{
		int sz = (t) ? img.rows : img.cols;
		Mat mhist = Mat::zeros(1, sz, CV_32F);

		for (int j = 0; j<sz; j++){
			Mat data = (t) ? img.row(j) : img.col(j);
			mhist.at<float>(j) = countNonZero(data);	//统计这一行或一列中，非零元素的个数，并保存到mhist中
		}

		//Normalize histogram
		double min, max;
		minMaxLoc(mhist, &min, &max);

		if (max>0)
			mhist.convertTo(mhist, -1, 1.0f / max, 0);//用mhist直方图中的最大值，归一化直方图

		return mhist;
	}

}	/* \namespace easypr  */