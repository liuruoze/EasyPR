EasyPR
======

EasyPR是一个中文的开源车牌识别系统，其目标是成为一个简单、高效、准确的车牌识别引擎。

相比于其他的车牌识别系统，EasyPR有如下特点：

* 它基于openCV这个开源库。这意味着你可以获取全部源代码，并且移植到opencv支持的所有平台。
* 它能够识别中文。例如车牌为苏EUK722的图片，它可以准确地输出std:string类型的"苏EUK722"的结果。
* 它的识别率较高。图片清晰情况下，车牌检测与字符识别可以达到80%以上的精度。

### 更新

本次更新是1.2版，主要改进在于提升了车牌定位模块的准确性，从70%左右到目前的94%，见下图：

![1.2版综合效果](doc/res/testresult.png)

主要改动如下：

* 车牌定位使用了“颜色信息”+“二次Sobel”的综合搜索方法。在下面的window中红框代表Sobel定位结果，黄框代表颜色定位结果。

* “批量测试”增加了一个结果查看window，这个窗口可以用SetDebug()方法开闭(true开，false关)。

![查看结果](doc/res/window.png)

* 基本攻克了“大角度定位”问题，下图的车牌被定位并转到了正确的视角。

![大角度定位](doc/res/bigangle.png)

* GDTS里新增了若干张新测试图，包括数张大角度图。

* “批量测试”结果现在同时会保存在“run_accuracy”文件中，可以查询历史信息。

* 与Linux版本做了整合，可以实现跨平台编译。

### 平台

目前除了windows平台以外，还有以下其他平台的EasyPR版本。一些平台的版本可能会暂时落后于主平台。

|版本 | 开发者 | 版本 | 地址 
|------|-------|-------|-------
| android |  goldriver  |  1.1  |  [linuxxx/EasyPR_Android](https://github.com/linuxxx/EasyPR_Android)
| linux | Micooz  |  1.2  |  已跟EasyPR整合
| ios | zhoushiwei |  1.1  |  [zhoushiwei/EasyPR-iOS](https://github.com/zhoushiwei/EasyPR-iOS)
| mac | zhoushiwei |  1.1  | [zhoushiwei/EasyPR](https://github.com/zhoushiwei/EasyPR)
| java | fan-wenjie |  1.2  | [fan-wenjie/EasyPR-Java](https://github.com/fan-wenjie/EasyPR-Java)

### 兼容性

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

使用Git克隆一份拷贝到你本机或者直接下载zip压缩吧。使用vs2010或以上版本的IDE选择“从现有代码创建项目”，引用EasyPR的目录。

以下表格是本工程中所有目录的解释:

|目录 | 解释
|------|----------
| src |  所有源文件
| model | 机器学习的模型
| train | 训练数据与说明
| image | 测试用的图片
| doc | 相关文档

以下表格是image目录中子目录的解释:

|目录 | 解释
|------|----------
| general_test | GDTS（通用数据测试集）
| natvie_test | NDTS（本地数据测试集）
| tmp | Debug模式下EasyPR输出中间图片的目录

以下表格是src目录中子目录的解释:

|目录 | 解释
|------|----------
| core |  核心功能
| include | 相关头文件
| test | 测试目录，包括单图测试与批量测试
| train | 训练目录，存放模型训练的代码
| util | 辅助功能

以下表格是src目录下一些核心文件的解释与关系:

|文件 | 解释
|------|----------
| plate_locate |  车牌定位
| plate_judge | 车牌判断
| plate_detect | 车牌检测，是车牌定位与车牌判断功能的组合
| chars_segment | 字符分割
| chars_identify | 字符鉴别
| chars_recognise | 字符识别，是字符分割与字符鉴别功能的组合
| plate_recognize | 车牌识别，是车牌检测与字符识别的共有子类
| features | 特征提取回调函数
| plate | 车牌抽象
| core_func.h | 共有的一些函数

以下表格是src目录下一些辅助文件的解释与关系:

|文件 | 解释
|------|----------
| util.h | 辅助功能头文件
| main.cpp | 主命令行窗口
| test.cpp | 单例测试
| accuracy_test.cpp | 批量测试
| svm_train.cpp | svm训练函数
| generate_gdts.cpp | GDTS生成函数

### Contributer

* liuruoze：作者与核心代码编写

* Micooz：linux平台编译，性能优化，util类

* jsxyhelu：deface版本一

* zhoushiwei：deface版本二

* ahccom：新的plateLocate函数

### 鸣谢

taotao1233，唐大侠，jsxyhelu，如果有一天(zhoushiwei)，学习奋斗，袁承志，圣城小石匠，goldriver，Micooz，梦里时光，Rain Wang，任薛纪，ahccom





