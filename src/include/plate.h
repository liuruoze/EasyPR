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

#include "prep.h"

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {
    
    class CPlate
    {
    public:
        bool bColored;
        CPlate();
        
        //! 设置与读取变量
        inline void setPlateMat(Mat param){ m_plateMat = param; }
        inline Mat getPlateMat() const{ return m_plateMat; }
        
        inline void setPlatePos(RotatedRect param){ m_platePos = param; }
        inline RotatedRect getPlatePos() const{ return m_platePos; }
        
        inline void setPlateStr(String param){ m_plateStr = param; }
        inline String getPlateStr() const{ return m_plateStr; }
        
    private:
        //! 车牌的图块
        Mat m_plateMat;
        
        //! 车牌在原图的位置
        RotatedRect m_platePos;
        
        //! 车牌字符串
        String m_plateStr;
    };
    
}	/*! \namespace easypr*/

#endif /* endif __PLATE_H__ */