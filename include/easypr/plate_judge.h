//////////////////////////////////////////////////////////////////////////
// Name:	    plate_judge Header
// Version:		1.1
// Date:	    2014-09-28
// MDate:	    2015-01-28
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:  
// Defines CPlateLocate
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_JUDGE_H__
#define __PLATE_JUDGE_H__

#include "plate.h"
#include "feature.h"
#include "core_func.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlateJudge {
 public:
  CPlateJudge();

  //! 对多幅车牌进行SVM判断
  int plateJudge(const std::vector<CPlate>&, std::vector<CPlate>&);

  //! 车牌判断
  int plateJudge(const std::vector<cv::Mat>&, std::vector<cv::Mat>&);

  //! 车牌判断（一副图像）
  int plateJudge(const cv::Mat& inMat, int& result);

  //! 直方图均衡
  cv::Mat histeq(cv::Mat);

  //! 装载SVM模型
  void LoadModel(const char* model);

 private:
  //！使用的SVM模型
  CvSVM svm;

  // ! EasyPR的getFeatures回调函数
  // ！用于从车牌的image生成svm的训练特征features
  svmCallback m_getFeatures;
};

}  /*! \namespace easypr*/

#endif /* endif __PLATE_JUDGE_H__ */