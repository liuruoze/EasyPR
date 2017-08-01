### Class CPlateLocate

#### Properties
 - (protected:)
   - [`int m_GaussianBlurSize`](#m_GaussianBlurSize)
   - [`int m_MorphSizeWidth`](#m_MorphSizeWidth)
   - [`int m_MorphSizeHeight`](#m_MorphSizeHeight)
   - [`float m_error`](#m_error)
   - [`float m_aspect`](#m_aspect)
   - [`int m_verifyMin`](#m_verifyMin)
   - [`int m_verifyMax`](#m_verifyMax)
   - [`int m_angle`](#m_angle)
   - [`bool m_debug`](#m_debug)

#### Functions
 - (public:)
   - [`bool CPlateLocate::verifySizes(RotatedRect mr)`](#verifySizes)
   - [`int CPlateLocate::mserSearch(const Mat &src,vector<Mat> &out,vector<vector<CPlate>>& out_plateVec, bool usePlateMser, vector<vector<RotatedRect>>& out_plateRRect,int img_index, bool showDebug)`](#mserSearch)
   - [`void CPlateLocate::setLifemode(bool param)`](#setLifemode)
   - [`int CPlateLocate::colorSearch(const Mat &src, const Color r, Mat &out,vector<RotatedRect> &outRects)`](#colorSearch)
   - [`int CPlateLocate::sobelFrtSearch(const Mat &src,vector<Rect_<float>> &outRects)`](#sobelFrtSearch)
   - [`int CPlateLocate::sobelOper(const Mat &in, Mat &out, int blurSize, int morphW, int morphH)`](#sobelOper)
   - [`int CPlateLocate::deskew(const Mat &src, const Mat &src_b,vector<RotatedRect> &inRects,vector<CPlate> &outPlates, bool useDeteleArea, Color color)`](#deskew)
   - [`bool CPlateLocate::isdeflection(const Mat &in, const double angle,double &slope)`](#isdeflection)
   - [`bool CPlateLocate::rotation(Mat &in, Mat &out, const Size rect_size, const Point2f center, const double angle)`](#rotation)
   - [`int CPlateLocate::plateColorLocate(Mat src, vector<CPlate> &candPlates,int index)`](#plateColorLocate)
   - [`int CPlateLocate::plateMserLocate(Mat src, vector<CPlate> &candPlates, int img_index)`](#plateMserLocate)
   - [`int CPlateLocate::sobelOperT(const Mat &in, Mat &out, int blurSize, int morphW,int morphH)`](#sobelOperT)
   - [`int CPlateLocate::plateSobelLocate(Mat src, vector<CPlate> &candPlates,int index)`](#plateSobelLocate)
   - [`int plateLocate(Mat, std::vector<Mat>&, int = 0)`](#plateLocate1)
   - [`int plateLocate(Mat, std::vector<CPlate>&, int = 0)`](#plateLocate2)

***
<a id="m_GaussianBlurSize"></a>
#### int m_GaussianBlurSize(`get|set`)
 - 高斯模糊所用的变量

<a id="m_MorphSizeWidth"></a>
#### int m_MorphSizeWidth(`get|set`)
 - 选定闭操作时的宽度

<a id="m_MorphSizeHeight"></a>
#### int m_MorphSizeHeight(`get|set`)
 - 选定闭操作时的长度

<a id="m_error"></a>
#### float m_error(`get|set`)
 - 允许的误差比

<a id="m_aspect"></a>
#### float m_aspect(`get|set`)
 - 车牌的长宽比

<a id="m_verifyMin"></a>
#### int m_verifyMin(`set`)
 - 筛选长宽比时的最小比例

<a id="m_verifyMax"></a>
#### int m_verifyMax(`set`)
 - 筛选长宽比时的最大比例

<a id="m_angle"></a>
#### int m_angle(`set`)
 - 角度判断所用变量,正负超过这个角度的筛选出来的矩形都会被舍弃

<a id="m_debug"></a>
#### bool m_debug(`get|set`)
 - 是否开启调试模式,0关闭,非0开启

***
<a id="verifySizes"></a>
#### bool CPlateLocate::verifySizes(RotatedRect mr)
 - 输入旋转矩阵`mr`
 - 函数内部设置了车牌的长宽比,通过长宽比来确定是不是车牌区域
 - 如果是,就返回true,否则就返回false

<a id="mserSearch"></a>
#### mserSearch(const Mat &src,vector<Mat> &out,vector<vector<CPlate>>& out_plateVec, bool usePlateMser, vector<vector<RotatedRect>>& out_plateRRect,int img_index, bool showDebug)
 - MSER区域提取原理:对图像取阈值在[0,255]之间的灰度化,当选取区域大小最为稳定的区域
 - <a href="http://blog.csdn.net/hust_bochu_xuchao/article/details/52230694">mser in opencv</a>
 - 该函数对输入图像进行了`MSER`区域提取并输出

<a id="setLifemode"></a>
#### void CPlateLocate::setLifemode(bool param)
 - 如果有`param`传入,对`GaussianBlurSize`等一系列参数进行设置
 - 如果没有参数传入,就采用默认的参数

<a id="colorSearch"></a>
#### int CPlateLocate::colorSearch(const Mat &src, const Color r, Mat &out,vector<RotatedRect> &outRects)
 - 此函数的作用是:对于输入的图片`src`和颜色`r`,对`src`进行`r`的颜色匹配
 - 匹配流程为(省略具体参数)
   - `colorMatch`:灰度化
   - `threshold`:二值化
   - `getStructuringElement`:规定`10x2`的`kernel`
   - `morphologyEx`:针对`kernel`进行闭操作(先erode再dilate)
   - `findContours`:闭操作完寻找轮廓以确定(蓝牌或黄牌)的区域
   - `out`里存储的是二值化结果,`outRects`里存储的是寻找到的区域
   - 调用`verifySizes`筛选出符合长宽比的区域
   - `return 0`

<a id="sobelFrtSearch"></a>
#### int CPlateLocate::sobelFrtSearch(const Mat &src,vector<Rect_<float>> &outRects)
 - 输入:
   - 原始图片`Mat &src`
   - 输出集合`vector<Rect_<float>> &outRects`
 - 执行过程
   - `sobelOper`:对图像进行预处理
   - `verifySizes`:对处理完的矩形集合进行筛选
   - 存储到outRects中

<a id="sobelOper"></a>
#### int CPlateLocate::sobelOper(const Mat &in, Mat &out, int blurSize, int morphW, int morphH)
 - 输入
   - 原始图片`Mat &in`
   - 输出图片`Mat &out`
   - 滤波窗口大小`blurSize`
   - 闭操作`kernel`的宽度`morphW`
   - 闭操作`kernel`的长度`morphH`
 - 函数流程(省略具体参数):
   - `GaussianBlur`:高斯滤波
   - `cvtColor`:灰度化
   - `Sobel`:sobel算子
   - `threshold`:二值化
   - `morphologyEx`:闭操作
   - 将结果`Mat`赋值到`out`里
   - `return 0`

<a id="deskew"></a>
#### int CPlateLocate::deskew(const Mat &src, const Mat &src_b,vector<RotatedRect> &inRects,vector<CPlate> &outPlates, bool useDeteleArea, Color color)
 - 输入:
   - `Mat &src`:原始图片
   - `Mat &src_b`:二值化后的图片
   - `vector<RotatedRect> &inRects`:闭操作识别颜色后的区域矩形块。
   - `vector<CPlate> &outPlates`:输出`vector`
   - `bool useDeteleArea`:
   - `Color color`:输入的颜色(蓝牌或者黄牌)
 - 对识别的区域进行旋转后,删除其中角度大于`m_angle`的,并对矩形进行划线(0,255,255)(BGR),之后存储到`CPlate类型中`,再保存到`outPlates`中

<a id="isdeflection"></a>
#### bool CPlateLocate::isdeflection(const Mat &in, const double angle,double &slope)
 - 判断是否倾斜

<a id="rotation"></a>
#### bool CPlateLocate::rotation(Mat &in, Mat &out, const Size rect_size, const Point2f center, const double angle)
 - 判断旋转

<a id="plateColorLocate"></a>
#### int CPlateLocate::plateColorLocate(Mat src, vector<CPlate> &candPlates,int index)
 - 输入:
   - `Mat src`:原始图片,`vector<CPlate> &candPlates`:存储颜色筛选完并旋转的图
   - 对`src`进行黄牌和蓝牌的并行计算
   - `return 0`

<a id="plateMserLocate"></a>
#### int CPlateLocate::plateMserLocate(Mat src, vector<CPlate> &candPlates, int img_index)
 - 基本操作类似于`color`和`sobel`
 - 采取的核心算法是`mser`
 - 最终结果存储在`candPlates`中

<a id="sobelOperT"></a>
#### int CPlateLocate::sobelOperT(const Mat &in, Mat &out, int blurSize, int morphW,int morphH)
 - 输入
   - 原始图片`Mat &in`
   - 输出图片`Mat &out`
   - 滤波窗口大小`blurSize`
   - 闭操作`kernel`的宽度`morphW`
   - 闭操作`kernel`的长度`morphH`
 - 处理等同于[`sobelOper`](#sobelOper),只不过在每一步操作时,都将分步的图片存储到了`/resources/image/tmp/`目录下

<a id="plateSobelLocate"></a>
#### int CPlateLocate::plateSobelLocate(Mat src, vector<CPlate> &candPlates,int index)
 - 采取sobel算子进行筛选
 - 输入
   - `Mat src`:输入图像
   - `vector<CPlate> &candPlates`:输出图像集合
 - 执行过程
   - `sobelFrtSearch`:矩形筛选
   - 放大矩形区域
   - 二次切割加入
   - `deskew`旋转筛选
   - 存储到`candPlates`中

<a id="plateLocate1"></a>
#### int plateLocate(Mat src, std::vector<Mat> &resultVec, int index = 0)
 - 输入`src`和`resultVec`
 - 将`src`进行车牌定位检测,检测到的结果放置到`resultVec`中
 - 检测过程如下:
  - `plateColorLocate` | `plateSobelLocate` | `plateMserLocate`
  - 三种检测手段得到的车牌候选矩形都会被放进`resultVec`中
 - `resultVec`中放置的类型是`Mat`
 - `return 0`

<a id="plateLocate2"></a>
#### int plateLocate(Mat src, std::vector<CPlate> &resultVec, int = 0)
 - 输入`src`和`resultVec`
 - 将`src`进行车牌定位检测,检测到的结果放置到`resultVec`中
 - `resultVec`中放置的类型是`CPlate`
 - `return 0`
