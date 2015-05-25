#ifndef __CORE_FUNC_H__
#define __CORE_FUNC_H__

#include <opencv2/opencv.hpp>

namespace easypr {

	enum Color{ BLUE, YELLOW };

	//! 根据一幅图像与颜色模板获取对应的二值图
	//! 输入RGB图像, 颜色模板（蓝色、黄色）
	//! 输出灰度图（只有0和255两个值，255代表匹配，0代表不匹配）
	cv::Mat colorMatch(const cv::Mat& src, cv::Mat& match, const Color r, const bool adaptive_minsv);

	//! 判断一个车牌的颜色
	//! 输入车牌mat与颜色模板
	//! 返回true或fasle
	bool plateColorJudge(const cv::Mat&  src, const Color r, const bool adaptive_minsv);

	bool bFindLeftRightBound(cv::Mat& bound_threshold,int& posLeft,int& posRight);
	bool bFindLeftRightBound1(cv::Mat& bound_threshold,int& posLeft,int& posRight);
	bool bFindLeftRightBound2(cv::Mat& bound_threshold,int& posLeft,int& posRight);
	
	//去除车牌上方的钮钉
	//计算每行元素的阶跃数，如果小于X认为是柳丁，将此行全部填0（涂黑）
	//X的推荐值为，可根据实际调整
	bool clearLiuDing(cv::Mat& img);
	void clearLiuDingOnly(cv::Mat& img);
	void clearLiuDing(cv::Mat mask,int& top,int& bottom);

	//! 获得车牌颜色
	Color getPlateType(const cv::Mat&  src, const bool adaptive_minsv);

	//! 直方图均衡
	cv::Mat histeq(cv::Mat in);
	cv::Mat features(cv::Mat in, int sizeData);
	int ThresholdOtsu(cv::Mat mat);


	// ！获取垂直和水平方向直方图
	cv::Mat ProjectedHistogram(cv::Mat img, int t);

}	/*! \namespace easypr*/

#endif
/* endif __CORE_FUNC_H__ */