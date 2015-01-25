EasyPR
======

EasyPR是一个中文的开源车牌识别系统，其目标是成为一个简单、高效、准确的车牌识别引擎。

相比于其他的车牌识别系统，EasyPR有如下特点：

* 它基于openCV这个开源库。这意味着你可以获取全部源代码，并且移植到java等平台。
* 它能够识别中文。例如车牌为苏EUK722的图片，它可以准确地输出std:string类型的"苏EUK722"的结果。
* 它的识别率较高。图片清晰情况下，车牌检测与字符识别可以达到90%以上的精度。

### 更新

目前EasyPR的版本是1.1，相比上一个版本，有以下更新。可以在[ChangeLog](doc/ChangeLog.md)中找到更多信息：
（这次的更新内容较多，为了跟你现有的项目和代码不冲突，请谨慎选择全部更新，最好新起一个目录试试新增的功能和内容）：

* svm准确率改进。使用rbf核替代liner核，在车牌判断的准确率提升了8个百分点。
* 增加两个特征提取方法，并提供了通用的回调函数接口供开发者建立新的特征。
* 增加了Debug模式，可以在image/tmp文件夹下看到所有输出的中间图片。
* 增加了LifeMode模式，相比默认模式，更适合在生活场景下定位车牌。
* 增加了批量测试功能，这个功能可供测试EasyPR在多幅图片上的整体表现效果。
* 引入了GDTS(General Data Test Set，通用数据测试集)这个概念，作为EasyPR准确率的评测数据集。
* 为了配合GDTS的数据仅用于正常途径(例如学习和研究)，引入了新的[GDSL协议](images/GDSL.txt)。
* 模块化与完善了SVM训练的功能。提供了一个方便简单的训练窗口。这些增加的功能是为了配合即将推出的SVM开发详解这篇文章。
* 完善了SVM训练的功能。引入了三个数据集的概念，即learn data，train data，test data。
* 使用了Precise，Recall，FSocre三个指标作为SVM模型准确率评判的参数与改善的依据。
* 引入了levenshtein距离作为EasyPR整体识别准确率误差的评判参数与改善依据。
* SVM训练数据中增加了数百张新数据(未经直方图均衡化的车牌图片和非车牌图片)
* 增加了一个命令行窗口，作为测试与训练的辅助工具。

注意：上一个版本中image文件下的test.jpg如果有同学下载了，请删除。它的格式不符合新的GDSL协议的约定。
如果想使用测试图片，可以使用1.1版本中新增的test.jpg替代。

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

以下表格是image目录下子目录的解释:

|目录 | 解释
|------|----------
| general_test | GDTS（通用数据测试集）
| natvie_test | NDTS（本地数据测试集）
| baidu_image | 从百度下载的图片
| tmp | Debug模式下EasyPR输出中间图片的目录

以下表格是src目录下子目录的解释:

|目录 | 解释
|------|----------
| core |  核心功能目录
| include | 相关头文件
| test | 测试目录，包括单图测试与批量测试
| train | 训练目录，存放训练数据
| util | 辅助功能目录

以下表格是src目录中一些核心文件的解释与关系:

|文件 | 解释
|------|----------
| plate_locate |  车牌定位
| plate_judge | 车牌判断
| plate_detect | 车牌检测，是车牌定位与车牌判断功能的组合
| chars_segment | 字符分割
| chars_identify | 字符鉴别
| chars_recognise | 字符识别，是字符分割与字符鉴别功能的组合
| plate_recognize | 车牌识别，是车牌检测与字符识别的共有子类
| features | 特征提取回调函数集合
| prep.h | 预包含头文件

以下表格是src目录中一些辅助文件的解释与关系:

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

taotao1233，唐大侠, jsxyhelu, 如果有一天, 学习奋斗, 袁承志, 圣城小石匠,





