#ifndef __FEATURE_H__
#define __FEATURE_H__

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr {
    
    //! 获得车牌的特征数
    Mat getTheFeatures(Mat in);
    
    //! EasyPR的getFeatures回调函数
    //! 用于从车牌的image生成svm的训练特征features
    typedef void(*svmCallback)(const Mat& image, Mat& features);
    
    //! EasyPR的getFeatures回调函数
    //! 本函数是生成直方图均衡特征的回调函数
    void getHisteqFeatures(const Mat& image, Mat& features);
    
    //!  EasyPR的getFeatures回调函数
    //! 本函数是获取垂直和水平的直方图图值
    void getHistogramFeatures(const Mat& image, Mat& features);
    
    //! 本函数是获取SIFT特征子
    void getSIFTFeatures(const Mat& image, Mat& features);
    
    //! 本函数是获取HOG特征子
    void getHOGFeatures(const Mat& image, Mat& features);
    
    //! 本函数是获取HSV空间量化的直方图特征子
    void getHSVHistFeatures(const Mat& image, Mat& features);
    
}	/*! \namespace easypr*/

#endif
/* endif __FEATURE_H__ */
