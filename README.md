EasyPR
======

EasyPR是一个中文的开源车牌识别系统，其目标是成为一个简单、高效、准确的车牌识别引擎。

目前EasyPR的版本是1.0，在未来会发布下一个版本：1.1。

相比于其他的车牌识别系统，EasyPR有如下特点：

* 它基于openCV这个开源库。这意味着你可以获取全部源代码，并且移植到java等平台。
* 它能够识别中文。例如车牌为苏EUK722的图片，它可以准确地输出std:string类型的"苏EUK722"的结果。
* 它的识别率较高。图片清晰情况下，车牌检测与字符识别可以达到90%以上的精度。

### 注意

EasyPR是基于opencv2.4.8版本开发的，2.4.8以上的版本应该可以兼容，以前的版本可能会存在不兼容的现象。opencv3.0的版本还没有经过测试。

### 例子

假设我们有如下的原始图片，需要识别出中间的车牌字符与颜色：

![EasyPR 原始图片](doc/res/plate_locate.jpg)

经过EasyPR的第一步处理车牌检测（PlateDetect）以后，我们获得了原始图片中仅包含车牌的图块：

![EasyPR 车牌](doc/res/blue_plate.jpg)

接着，我们对图块进行OCR过程，在EasyPR中，叫做字符识别（CharsRecognize）。我们得到了一个包含车牌颜色与字符的字符串：

“蓝牌：苏EUK722”


### 安装

EasyPR不需要安装，开发者直接在其上做改动。如果想使用DLL形式引用或者使用其他语言调用，则可以在[EasyPR_DLL_src](https://github.com/liuruoze/EasyPR_Dll_src)中找到。

详细的开发与教程请见[介绍与开发教程](http://www.cnblogs.com/subconscious/p/3979988.html)。

### 使用

使用Git克隆一份拷贝到你本机或者直接下载zip压缩吧。使用vs2010或以上版本的IDE选择“从现有代码创建项目”，引用EasyPR的目录（第一次运行请排除ann和svm两个训练cpp）。

以下表格是本工程中所有目录的解释与关系:

|目录 | 解释
|------|----------
| src |  所有源文件
| model | 机器学习的模型
| train | 训练代码与数据
| image | 测试用的图片
| doc | 相关文档
| tmp | 存放临时图片的目录

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

### 问题

如果有任何问题或者建议请在issues里直接提交，或者发作者的email：liuruoze@163.com。
建议与问题一经采纳即会将您的贡献大名列入EasyPR的感谢名单（ Credits ）中。

### 鸣谢

taotao1233，唐大侠





