EasyPR
======

EasyPR是一个中文的开源车牌识别系统，其目标是成为一个简单、高效、准确的车牌识别引擎。

相比于其他的车牌识别系统，EasyPR有如下特点：

* 它基于openCV这个开源库。这意味着你可以获取全部源代码，并且移植到java等平台。
* 它能够识别中文。例如车牌为苏EUK722的图片，它可以准确地输出std:string类型的"苏EUK722"的结果。
* 它的识别率较高。图片清晰情况下，车牌检测与字符识别可以达到90%以上的精度。

### 版本

EasyPR目前除了windows版以外，还有以下几个版本：

* **android**:  EasyPR的android版本。由goldriver开发，地址在[EasyPR_Android](https://github.com/linuxxx/EasyPR_Android)（感谢goldriver同学）。
* **linux**:    EasyPR的linux版本。由Micooz开发，地址在[linux-dev](https://github.com/Micooz/EasyPR/tree/linux-dev)（感谢Micooz同学）。
* **ios**:      正在开发中。

### 更新

目前EasyPR的版本是1.1，相比上一个版本，有以下更新。可以在[ChangeLog](doc/ChangeLog.md)中找到更多信息：
（这次的更新内容较多，为了跟你现有的项目和代码不冲突，请谨慎选择全部更新，最好新起一个目录试试新增的功能和内容）：

* 新的SVM模型。新模型使用rbf核替代了liner核，在车牌判断的准确率提升了8个百分点。
* 新增两个特征提取方法。并提供了相关的回调函数接口。
* 新增Debug模式。可以在image/tmp文件夹下看到所有输出的中间图片。
* 新增LifeMode模式。相比默认模式，更适合在生活场景下定位车牌。
* 新增批量测试功能。这个功能可供测试EasyPR在多幅图片上的整体表现效果。
* 引入GDTS(General Data Test Set。通用数据测试集)概念，作为EasyPR准确率的评测数据集。
* 引入[GDSL协议](image/GDSL.txt)。此协议是为了确保GDTS中的数据不受到任何商业性与恶性目的行为的滥用。
* 完善SVM训练功能。提供了一个方便的训练操作窗口。这些功能是为了配合即将发布的SVM开发详解这篇文章。
* 强化SVM模型验证。使用了三个新的数据集概念，即learn data，train data，test data。
* 新增评价指标。引入Precise，Recall，FSocre三个指标这三个指标作为SVM模型准确率评判的参数与改善的依据。
* 新增整体指标。引入levenshtein距离作为EasyPR整体识别准确率误差的评判参数与改善依据。
* 大幅增加训练数据。SVM训练数据中增加了近千张新数据(未经直方图均衡化的车牌图片和非车牌图片)。
* 新增命令行窗口，作为测试与训练的辅助工具。

注意：上一个版本中image文件下的test.jpg请删除。它的格式已经不符合新的[GDSL协议](image/GDSL.txt)的约定。
如果想测试，可以使用本版本中替换的test.jpg。

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

taotao1233，唐大侠，jsxyhelu，如果有一天，学习奋斗，袁承志，圣城小石匠，





