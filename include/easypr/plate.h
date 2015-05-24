//////////////////////////////////////////////////////////////////////////
// Name:	    plate Header
// Version:		1.0
// Date:	    2015-03-12
// Author:	    liuruoze
// Copyright:   liuruoze
// Desciption:  
// Defines CPlate
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_H__
#define __PLATE_H__

#include <opencv2/opencv.hpp>

namespace easypr {

class CPlate {
 public:
  bool bColored;

  CPlate();

  //! 设置与读取变量
  inline void setPlateMat(cv::Mat param) { m_plateMat = param; }

  inline cv::Mat getPlateMat() const { return m_plateMat; }

  inline void setPlatePos(cv::RotatedRect param) { m_platePos = param; }

  inline cv::RotatedRect getPlatePos() const { return m_platePos; }

  inline void setPlateStr(std::string param) { m_plateStr = param; }

  inline std::string getPlateStr() const { return m_plateStr; }

 private:
  //! 车牌的图块
  cv::Mat m_plateMat;

  //! 车牌在原图的位置
  cv::RotatedRect m_platePos;

  //! 车牌字符串
  std::string m_plateStr;
};

}  /*! \namespace easypr*/

#endif /* endif __PLATE_H__ */