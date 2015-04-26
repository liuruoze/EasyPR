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

#include "prep.h"

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {
    
    class CCharsIdentify
    {
    public:
        CCharsIdentify();
        
        //! 字符鉴别
        string charsIdentify(Mat, bool, bool);
        string charsIdentify(Mat input);
        
        //! 字符分类
        int classify(Mat, bool,bool);
        int classify(Mat f);
        //! 装载ANN模型
        void LoadModel();
        
        //! 装载ANN模型
        void LoadModel(string s);
        
        //! 设置与读取模型路径
        inline void setModelPath(string path){	m_path = path;	}
        inline string getModelPath() const{	 return m_path;	}
        
    private:
        //！使用的ANN模型
        CvANN_MLP ann;
        
        //! 模型存储路径
        string m_path;
        
        //! 特征尺寸
        int m_predictSize;
        
        //! 省份对应map
        map<string, string> m_map;
    };
    
}	/* \namespace easypr  */

#endif /* endif __CHARS_IDENTIFY_H__ */