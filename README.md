EasyPR
======

EasyPR是一个中文的开源车牌识别系统，其目标是成为一个简单、高效、准确的车牌识别引擎。

相比于其他的车牌识别系统，EasyPR有如下特点：

* 它基于openCV这个开源库。这意味着你可以获取全部源代码，并且移植到java等平台。
* 它能够识别中文。例如车牌为苏EUK722的图片，它可以准确地输出std:string类型的"苏EUK722"的结果。
* 它的识别率较高。图片清晰情况下，车牌检测与字符识别可以达到90%以上的精度。

### 版本

EasyPR最开始是发布在[GitHub](https://github.com/liuruoze/EasyPR)上的，然后在国内的[oschina](http://git.oschina.net/easypr/EasyPR)上也部署了一份镜像。
相关的issue欢迎在GitHub上统一提交。目前除了windows版本以外，还有以下其他平台的版本：

|版本 | 开发者 | 地址
|------|-------|-------
| android |  goldriver  |  [linuxxx/EasyPR_Android](https://github.com/linuxxx/EasyPR_Android)
| linux | Micooz  |  [Micooz/EasyPR/tree/linux-dev](https://github.com/Micooz/EasyPR/tree/linux-dev)
| ios | zhoushiwei |  [zhoushiwei/EasyPR-iOS](https://github.com/zhoushiwei/EasyPR-iOS)
| mac | zhoushiwei | [zhoushiwei/EasyPR](https://github.com/zhoushiwei/EasyPR)
| c# | 招聘中 |   

感谢以上所有开发者的努力！

### 更新

* 本版本是EasyPR 1.2版本，相比前版本的主要改动是明显提升了车牌定位模块的准确性，主要内容如下：
* 1.车牌定位使用了“颜色信息”+“二次Sobel”的综合搜索方法。
* 2.“批量测试”增加了观看结果图片的功能，这个功能可以用SetDebug()方法开关(true开，false关)。
* 3.攻克了“大角度定位”问题。
* 4.GDTS里新增了若干张新测试图，包括数张大角度图。

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
| baidu_image | 从百度下载的图片
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
| prep.h | 预包含头文件

以下表格是src目录下一些辅助文件的解释与关系:

|文件 | 解释
|------|----------
| util.h | 辅助功能头文件
| main.cpp | 主命令行窗口
| test.cpp | 单例测试
| accuracy_test.cpp | 批量测试
| svm_train.cpp | svm训练函数
| generate_gdts.cpp | GDTS生成函数

### 问题

如果有任何问题或者建议请在issues里直接提交，或者发email：easypr_dev@163.com。
建议与问题一经采纳即会将您的贡献大名列入EasyPR的感谢名单（ Credits ）中。

### 鸣谢

taotao1233，唐大侠，jsxyhelu，如果有一天(zhoushiwei)，学习奋斗，袁承志，圣城小石匠，goldriver，Micooz，梦里时光，Rain Wang，任薛纪





