EasyPR
======
http://www.cnblogs.com/subconscious/p/3979988.html

EasyPR是一个中文的开源车牌识别系统。EasyPR的意思是Easy to do Plate Recognition。EasyPR的目标是成为一个简单的，灵活的，准确的免费车牌识别引擎。

**———— 大数据时代，车牌系统会有新的定义和挑战！**

### 特征

相比于其他的车牌识别系统，EasyPR有如下特点：
* 它基于openCV这个开源库，这意味着所有它的代码都可以轻易的获取。
* 它能够识别中文，这是它的一个重要特点。
* 它的识别率还可以，考虑到它还有很多待改进和优化的空间，这已经很不错了。

### 样例

假设我们有如下的原始图片：

![EasyPR 原始图片](doc/res/plate_locate.jpg)

经过EasyPR的第一步处理车牌检测（PlateDetect）以后，我们获得了原始图片中仅包含车牌的图块：

![EasyPR 车牌](doc/res/blue_plate.jpg)

接着，我们对图块进行OCR过程，在EasyPR中，叫做字符识别（CharsRecognize）。我们得到了一个包含车牌颜色与字符的字符串：

“蓝牌：苏EUK722”

![EasyPR 字符](doc/res/blue_chars.png.png)

上面的例子举的是一个蓝牌的私家车，如果换成黄牌的大货车：

![EasyPR 原始图片](doc/res/plate_detect.jpg)

经过EasyPR的第一步处理车牌检测以后，我们获得了原始图片中仅包含车牌的图块：

![EasyPR 原始图片](doc/res/yewllow_plate.jpg)

接着，进行字符识别。我们得到了一个包含车牌颜色与字符的字符串：

“蓝牌：苏EUK722”

![EasyPR 字符](doc/res/yellow_chars.png)

### 架构

EasyPR把车牌识别划分为了两个过程：即车牌检测（Plate Detection）和字符识别（Chars Recognition）两个过程。可能有些书籍或论文上不是这样叫的，但这样的叫法更容易理解，也不容易搞混。

* 车牌检测（Plate Detection）：对一个包含车牌的图像进行分析，最终截取出只包含车牌的一个图块。这个步骤的主要目的是降低了在车牌识别过程中的计算量。如果直接对原始的图像进行车牌识别，会非常的慢，因此需要检测的过程。在本系统中，我们使用SVM（支持向量机）这个机器学习算法去判别截取的图块是否是真的“车牌”。
* 字符识别（Chars Recognition）：有的书上也叫Plate Recognition，我为了与整个系统的名称做区分，所以改为此名字。这个步骤的主要目的就是从上一个车牌检测步骤中获取到的车牌图像，进行光学字符识别（OCR）这个过程。其中用到的机器学习算法是著名的人工神经网络（ANN）中的多层感知机（MLP）模型。最近一段时间非常火的“深度学习”其实就是多隐层的人工神经网络，与其有非常紧密的联系。通过了解光学字符识别（OCR）这个过程，也可以知晓深度学习所基于的人工神经网路技术的一些内容。



### 安装

EasyPR不需要安装，开发者直接在其上做改动。如果想使用DLL形式引用或者使用C#与Java调用，则可以在[EasyPR_DLL](https://github.com/liuruoze/EasyPR_Dll)中找到。



### 使用

使用Git克隆一份拷贝到你本机或者直接下载zip压缩吧。使用vs2010或以上版本的IDE打开sln文件后，就可以看到本项目的所有目录。

以下表格是本工程中所有目录的解释与关系:

|目录 | 解释
|------|----------
| src |  所有源文件
| model | 机器学习的模型
| train | 训练代码与数据
| image | 测试用的图片
| doc | 相关文档

以下表格是src目录中所有文件的解释与关系:

|文件 | 解释
|------|----------
| plate_locate |  车牌定位
| plate_judge | 车牌判断
| plate_detect | 车牌检测，是车牌定位与车牌判断功能的组合
| chars_segment | 字符分割
| chars_identify | 字符鉴别
| chars_recognise | 字符识别，是字符分割与字符鉴别功能的组合
| plate_recognize | 车牌识别，是车牌检测与字符识别的共有子类
| prep.h | 预包含头文件
| test.cpp | 测试
