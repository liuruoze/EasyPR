//////////////////////////////////////////////////////////////////////////
// Name:	    CHARACTER Header
// Version:		1.0
// Date:	    2016-06-14
// Author:	    liuruoze
// Copyright:   liuruoze
// Desciption:
// An abstract class for car character in plate.
//////////////////////////////////////////////////////////////////////////
#ifndef EASYPR_CORE_CHARACTER_H_
#define EASYPR_CORE_CHARACTER_H_
#include "core_func.h"

/*! \namespace easypr
Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

  class CCharacter {
  public:
    CCharacter()
    {
    }

    CCharacter(const CCharacter& other)
    {
      m_characterMat = other.m_characterMat;
      m_characterPos = other.m_characterPos;
      m_characterStr = other.m_characterStr;
      m_score = other.m_score;
    }

    inline void setCharacterMat(Mat param) { m_characterMat = param; }
    inline Mat getCharacterMat() const { return m_characterMat; }

    inline void setCharacterPos(Rect param) { m_characterPos = param; }
    inline Rect getCharacterPos() const { return m_characterPos; }

    inline void setCharacterStr(String param) { m_characterStr = param; }
    inline String getCharacterStr() const { return m_characterStr; }

    inline void setCharacterScore(double param) { m_score = param; }
    inline double getCharacterScore() const { return m_score; }

  private:
    //! character mat

    Mat m_characterMat;

    //! character rect

    Rect m_characterPos;

    //! character str

    String m_characterStr;

    //! character likely

    double m_score;

  };

} /*! \namespace easypr*/

#endif  // EASYPR_CORE_PLATE_H_