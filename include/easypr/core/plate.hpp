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

#include "easypr/core/core_func.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  class CPlate {
  public:
    CPlate() { }

    CPlate(const CPlate& plate)
    {
      m_plateMat = plate.m_plateMat;
      m_score = plate.m_score;
      m_platePos = plate.m_platePos;
      m_plateStr = plate.m_plateStr;
      m_locateType = plate.m_locateType;
    }

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
    //! plate mat

    Mat m_plateMat;

    //! plate rect

    RotatedRect m_platePos;

    //! plate license

    String m_plateStr;

    //! plate locate type

    LocateType m_locateType;

    //! plate likely

    double m_score;

  };

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_PLATE_H_