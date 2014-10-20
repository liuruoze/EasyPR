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

假设我们有如下的图片：
![EasyPR 原始图片](image/plate_detect.jpg)

经过EasyPR的第一步处理PlateDetect以后：


再经过EasyPR的第二部处理CharsRecognize以后：



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
