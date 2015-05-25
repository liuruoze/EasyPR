//////////////////////////////////////////////////////////////////////////
// Name:	    chars_identify Header
// Version:		1.0
// Date:	    2014-09-25
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CCharsIdentify 
//////////////////////////////////////////////////////////////////////////
#ifndef __CHARS_IDENTIFY_H__
#define __CHARS_IDENTIFY_H__

#include <opencv2/opencv.hpp>
#include <string>

namespace easypr {

class CCharsIdentify {
 public:
  CCharsIdentify();

  //! 字符鉴别
  std::string charsIdentify(cv::Mat, bool, bool);

  std::string charsIdentify(cv::Mat input);

  //! 字符分类
  int classify(cv::Mat, bool, bool);

  int classify(cv::Mat f);

  //! 装载ANN模型
  void LoadModel();

  //! 装载ANN模型
  void LoadModel(std::string s);

  //! 设置与读取模型路径
  inline void setModelPath(std::string path) { m_path = path; }

  inline std::string getModelPath() const { return m_path; }

 private:
  //！使用的ANN模型
  CvANN_MLP ann;

  //! 模型存储路径
  std::string m_path;

  //! 特征尺寸
  int m_predictSize;

  //! 省份对应map
  std::map<std::string, std::string> m_map;
};

}  /* \namespace easypr  */

#endif /* endif __CHARS_IDENTIFY_H__ */