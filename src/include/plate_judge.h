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

#include "prep.h"
#include "plate.h"
#include "feature.h"
#include "core_func.h"


/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {
    
    class CPlateJudge
    {
    public:
        CPlateJudge();
        
        //! 对多幅车牌进行SVM判断
        int plateJudge(const vector<CPlate>&, vector<CPlate>&);
        
        //! 车牌判断
        int plateJudge(const vector<Mat>&, vector<Mat>&);
        
        //! 车牌判断（一副图像）
        int plateJudge(const Mat& inMat, int& result);
        
        //! 直方图均衡
        Mat histeq(Mat);
        
        //! 装载SVM模型
        void LoadModel();
        
        //! 装载SVM模型
        void LoadModel(string s);
        
        //! 设置与读取模型路径
        inline void setModelPath(string path){	m_path = path;	}
        inline string getModelPath() const{	 return m_path;	}
        
    private:
        //！使用的SVM模型
        CvSVM svm;
        
        // ! EasyPR的getFeatures回调函数
        // ！用于从车牌的image生成svm的训练特征features
        svmCallback m_getFeatures;
        
        //! 模型存储路径
        string m_path;
    };
    
}	/*! \namespace easypr*/

#endif /* endif __PLATE_JUDGE_H__ */