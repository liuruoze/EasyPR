//////////////////////////////////////////////////////////////////////////
// Name:	    plate Header
// Version:		1.0
// Date:	    2015-03-12
// Author:	    liuruoze
// Copyright:   liuruoze
// Desciption:
// An abstract class for car plate.
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_PLATE_H_
#define EASYPR_CORE_PLATE_H_
#include "core_func.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

class CPlate {
 public:
  bool bColored;

  //! 构造函数

  CPlate();

  CPlate(const CPlate& plate)
  {
    m_plateMat = plate.m_plateMat;
    m_score = plate.m_score;
    m_platePos = plate.m_platePos;
    m_plateStr = plate.m_plateStr;
    m_locateType = plate.m_locateType;
  }


  //! 设置与读取变量

  inline void setPlateMat(Mat param) { m_plateMat = param; }
  inline Mat getPlateMat() const { return m_plateMat; }

  inline void setPlatePos(RotatedRect param) { m_platePos = param; }
  inline RotatedRect getPlatePos() const { return m_platePos; }

  inline void setPlateStr(String param) { m_plateStr = param; }
  inline String getPlateStr() const { return m_plateStr; }

  inline void setPlateLocateType(LocateType param) { m_locateType = param; }
  inline LocateType getPlateLocateType() const { return m_locateType; }

  inline void setPlateScore(double param) { m_score = param; }
  inline double getPlateScore() const { return m_score; }

  bool operator < (const CPlate& plate) const
  {
    return (m_score < plate.m_score);
  }

  bool operator < (const CPlate& plate) 
  {
    return (m_score < plate.m_score);
  }

 private:
  //! 车牌的图块

  Mat m_plateMat;

  //! 车牌在原图的位置

  RotatedRect m_platePos;

  //! 车牌字符串

  String m_plateStr;

  //! 车牌定位的方法

  LocateType m_locateType;

  //! 车牌的置信度

  double m_score;

};

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_PLATE_H_