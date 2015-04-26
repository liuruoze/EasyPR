//////////////////////////////////////////////////////////////////////////
// Name:	    plate_detect Header
// Version:		1.2
// Date:	    2014-09-28
// MDate:	    2015-03-13
// Author:	    liuruoze
// Copyright:   liuruoze
// Reference:	Mastering OpenCV with Practical Computer Vision Projects
// Reference:	CSDN Bloger taotao1233
// Desciption:
// Defines CPlateDetect
//////////////////////////////////////////////////////////////////////////
#ifndef __PLATE_DETECT_H__
#define __PLATE_DETECT_H__

#include "prep.h"

#include "plate_locate.h"
#include "plate_judge.h"


/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {
    
    class CPlateDetect
    {
    public:
        CPlateDetect();
        
        //! 车牌检测：车牌定位与判断
        int plateDetect(Mat, vector<Mat>&, int index = 0);
        
        //! 深度车牌检测，使用颜色与二次Sobel法综合
        int plateDetectDeep(Mat src, vector<CPlate>& resultVec, bool showDetectArea = true, int index = 0);
        
        int showResult(const Mat& result);
        
        //! 装载SVM模型
        void LoadSVM(string s);
        
        //! 生活模式与工业模式切换
        inline void setPDLifemode(bool param){m_plateLocate->setLifemode(param);}
        
        //! 是否开启调试模式
        inline void setPDDebug(int param){ m_plateLocate->setDebug(param);}
        
        //! 获取调试模式状态
        inline int getPDDebug(){ return m_plateLocate->getDebug();}
        
        //! 设置与读取变量
        inline void setGaussianBlurSize(int param){	m_plateLocate->setGaussianBlurSize(param);}
        inline int getGaussianBlurSize() const{	return m_plateLocate->getGaussianBlurSize();}
        
        inline void setMorphSizeWidth(int param){m_plateLocate->setMorphSizeWidth(param);}
        inline int getMorphSizeWidth() const{return m_plateLocate->getMorphSizeWidth();}
        
        inline void setMorphSizeHeight(int param){m_plateLocate->setMorphSizeHeight(param);}
        inline int getMorphSizeHeight() const{return m_plateLocate->getMorphSizeHeight();}
        
        inline void setVerifyError(float param){m_plateLocate->setVerifyError(param);}
        inline float getVerifyError() const { return m_plateLocate->getVerifyError();}
        inline void setVerifyAspect(float param){m_plateLocate->setVerifyAspect(param);}
        inline float getVerifyAspect() const { return m_plateLocate->getVerifyAspect();}
        
        inline void setVerifyMin(int param){m_plateLocate->setVerifyMin(param);}
        inline void setVerifyMax(int param){m_plateLocate->setVerifyMax(param);}
        
        inline void setJudgeAngle(int param){m_plateLocate->setJudgeAngle(param);}
        
        inline void setMaxPlates(float param){ m_maxPlates = param; }
        inline float getMaxPlates() const { return m_maxPlates; }
        
    private:
        //! 设置一幅图中最多有多少车牌
        int m_maxPlates;
        
        //! 车牌定位
        CPlateLocate* m_plateLocate;
        
        //! 车牌判断
        CPlateJudge* m_plateJudge;
    };
    
}	/*! \namespace easypr*/

#endif /* endif __PLATE_DETECT_H__ */