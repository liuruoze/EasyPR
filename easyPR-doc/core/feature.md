### feature

#### Functions
 - [`cv::Mat getHistogram(cv::Mat in)`](#getHistogram)
 - [`typedef void (*svmCallback)(const cv::Mat& image, cv::Mat& features)`](#svmCallback)
 - [`typedef void (*annCallback)(const cv::Mat& image, cv::Mat& features)`](#annCallback)
 - [`void getGrayPlusProject(const cv::Mat& grayChar, cv::Mat& features)`](#getGrayPlusProject)
 - [`void getHistogramFeatures(const cv::Mat& image, cv::Mat& features)`](#getHistogramFeatures)
 - [`void getSIFTFeatures(const cv::Mat& image, cv::Mat& features)`](#getSIFTFeatures)
 - [`void getHOGFeatures(const cv::Mat& image, cv::Mat& features)`](#getHOGFeatures)
 - [`void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features)`](#getHSVHistFeatures)
 - [`void getLBPFeatures(const cv::Mat& image, cv::Mat& features)`](#getLBPFeatures)
 - [`void getColorFeatures(const cv::Mat& src, cv::Mat& features)`](#getColorFeatures)
 - [`void getHistomPlusColoFeatures(const cv::Mat& image, cv::Mat& features)`](#getHistomPlusColoFeatures)
 - [`cv::Mat charFeatures(cv::Mat in, int sizeData)`](#charFeatures)
 - [`cv::Mat charFeatures2(cv::Mat in, int sizeData)`](#charFeatures2)
 - [`void getLBPplusHistFeatures(const cv::Mat& image, cv::Mat& features)`](#getLBPplusHistFeatures)
 - [`void getGrayCharFeatures(const cv::Mat& grayChar, cv::Mat& features)`](#getGrayCharFeatures)
 - [`void getGrayPlusLBP(const Mat& grayChar, Mat& features)`](#getGrayPlusLBP)

***
<a id="getHistogram"></a>
#### cv::Mat getHistogram(cv::Mat in)
 - 获得车牌的特征数

<a id="svmCallback"></a>
#### typedef void (*svmCallback)(const cv::Mat& image, cv::Mat& features)
 - EasyPR的getFeatures回调函数
 - 用于从车牌的image生成svm的训练特征features

<a id="annCallback"></a>
#### typedef void (*annCallback)(const cv::Mat& image, cv::Mat& features)
 - EasyPR的getFeatures回调函数
 - convert from images to features used by gray char ann

<a id="getGrayPlusProject"></a>
#### void getGrayPlusProject(const cv::Mat& grayChar, cv::Mat& features)
 - gray and project feature

<a id="getHistogramFeatures"></a>
#### void getHistogramFeatures(const cv::Mat& image, cv::Mat& features)
 - EasyPR的getFeatures回调函数
 - 本函数是获取垂直和水平的直方图图值

<a id="getSIFTFeatures"></a>
#### void getSIFTFeatures(const cv::Mat& image, cv::Mat& features)
 - 本函数是获取SIFT特征子

<a id="getHOGFeatures"></a>
#### void getHOGFeatures(const cv::Mat& image, cv::Mat& features)
 - 本函数是获取HOG特征子

<a id="getHSVHistFeatures"></a>
#### void getHSVHistFeatures(const cv::Mat& image, cv::Mat& features)
 - 本函数是获取HSV空间量化的直方图特征子

<a id="getLBPFeatures"></a>
#### void getLBPFeatures(const cv::Mat& image, cv::Mat& features)
 - LBP feature

<a id="getColorFeatures"></a>
#### void getColorFeatures(const cv::Mat& src, cv::Mat& features)
 - color feature

<a id="getHistomPlusColoFeatures"></a>
#### void getHistomPlusColoFeatures(const cv::Mat& image, cv::Mat& features)
 - color feature and histom

<a id="charFeatures"></a>
#### cv::Mat charFeatures(cv::Mat in, int sizeData)
 - get character feature

<a id="charFeatures2"></a>
#### cv::Mat charFeatures2(cv::Mat in, int sizeData)
 - get character feature

<a id="getLBPplusHistFeatures"></a>
#### void getLBPplusHistFeatures(const cv::Mat& image, cv::Mat& features)
 - LBP feature + Histom feature

<a id="getGrayCharFeatures"></a>
#### void getGrayCharFeatures(const cv::Mat& grayChar, cv::Mat& features)

<a id="getGrayPlusLBP"></a>
#### void getGrayPlusLBP(const Mat& grayChar, Mat& features)
