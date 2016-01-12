# EasyPR

EasyPR是一个中文的开源车牌识别系统，其目标是成为一个简单、高效、准确的车牌识别引擎。

相比于其他的车牌识别系统，EasyPR有如下特点：

* 它基于openCV这个开源库。这意味着你可以获取全部源代码，并且移植到opencv支持的所有平台。
* 它能够识别中文。例如车牌为苏EUK722的图片，它可以准确地输出std:string类型的"苏EUK722"的结果。
* 它的识别率较高。图片清晰情况下，车牌检测与字符识别可以达到80%以上的精度。

### 更新

本次更新是1.4版，主要改进在于几个方面：

1.支持opencv3.0与3.1，注意，这与opencv2.x不兼容，要想支持的话请下载1.3版本。

2.ANN训练开放。

3.修正了SVM训练异常的问题。

4.代码优化。

不知道怎么下载以前的版本的小伙伴可以在github或gitosc的"branch"里选择"tags"，然后点击"v1.3"，再然后点击"download zip"。当然如果直接git clone的话可以随时方便切换。

在后面的版本中计划做以下几点改善：

1.新的评价框架，更加合理的评估数据。

2.新的车牌定位算法。

### 跨平台

目前除了windows平台以外，还有以下其他平台的EasyPR版本。一些平台的版本可能会暂时落后于主平台。

|版本 | 开发者 | 版本 | 地址 
|------|-------|-------|-------
| android |  goldriver  |  1.3  |  [linuxxx/EasyPR_Android](https://github.com/linuxxx/EasyPR_Android)
| linux | Micooz  |  1.4  |  已跟EasyPR整合
| ios | zhoushiwei |  1.3  |  [zhoushiwei/EasyPR-iOS](https://github.com/zhoushiwei/EasyPR-iOS)
| mac | zhoushiwei,Micooz |  1.4  | 已跟EasyPR整合
| java | fan-wenjie |  1.2  | [fan-wenjie/EasyPR-Java](https://github.com/fan-wenjie/EasyPR-Java)

### 兼容性

当前EasyPR是基于opencv3.0版本开发的，3.0及以上的版本应该可以兼容，以前的版本可能会存在不兼容的现象。

### 例子

假设我们有如下的原始图片，需要识别出中间的车牌字符与颜色：

![EasyPR 原始图片](resources/doc/res/plate_locate.jpg)

经过EasyPR的第一步处理车牌检测（PlateDetect）以后，我们获得了原始图片中仅包含车牌的图块：

![EasyPR 车牌](resources/doc/res/blue_plate.jpg)

接着，我们对图块进行OCR过程，在EasyPR中，叫做字符识别（CharsRecognize）。我们得到了一个包含车牌颜色与字符的字符串：

“蓝牌：苏EUK722”

### 版权

EasyPR的源代码与训练数据遵循Apache v2.0协议开源。

EasyPR的resources/image/general_test文件夹下的图片数据遵循[GDSL协议](image/GDSL.txt)（通用数据共享协议）进行开放。

请确保在使用前了解以上协议的内容。

### 目录结构

以下表格是本工程中所有目录的解释:

|目录 | 解释
|------|----------
| src |  所有源文件
| include | 所有头文件
| test | 测试程序
| resources/model | 机器学习的模型
| resources/train | 训练数据与说明
| resources/image | 测试用的图片
| resources/doc | 相关文档

以下表格是resources/image目录中子目录的解释:

|目录 | 解释
|------|----------
| general_test | GDTS（通用数据测试集）
| native_test | NDTS（本地数据测试集）
| tmp | Debug模式下EasyPR输出中间图片的目录

以下表格是src目录中子目录的解释:

|目录 | 解释
|------|----------
| core |  核心功能
| preprocess | SVM预处理
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
| feature | 特征提取回调函数
| plate | 车牌抽象
| core_func.h | 共有的一些函数

以下表格是test目录下文件的解释:

|文件 | 解释
|------|----------
| main.cpp | 主命令行窗口
| accuracy.hpp | 批量测试
| chars.hpp | 字符识别相关
| plate.hpp | 车牌识别相关

### 使用

EasyPR的所有源代码可在Github上的[项目主页](https://github.com/liuruoze/EasyPR)直接打包下载得到。

由于Github在中国有时下载速度较慢，可以使用oschina的镜像[地址](http://git.oschina.net/easypr/EasyPR)来下载。

如果你熟悉git版本控制工具，可以使用下面的命令来从Github里克隆代码：

    $ git clone https://github.com/liuruoze/EasyPR

EasyPR支持当前主流的操作系统，通常不需要对源代码进行更改就可以编译运行，尽管如此，不同平台上IDE的配置也是有很大差异的，下面主要说明Windows，Linux以及Mac OS下的编译方法。

**Note**: 无论在哪个平台使用EasyPR，首先都要安装对应平台版本的[opencv](http://opencv.org/)，建议使用正式稳定版本。

#### Windows

Windows下的配置建议使用最新的**Visual Studio 2013**版本。目前opencv3.0已经不支持vs2010，vs2012对C++ 11支持的不足，会存在编译问题。

在之前版本的使用和交流过程中，我们发现很多同学对Visual Studio开发环境不甚了解，甚至没有做过C++项目，对EasyPR环境的配置存在很大的障碍。为此在新版本中我们特意准备了傻瓜式自动配置脚本，来方便大家使用EasyPR。

**方法一（推荐）**

1. 首先你需要安装最新版本的[Python](https://www.python.org/downloads/)。将Python的安装目录（默认安装在C:\Python34）添加到系统环境变量PATH中。
2. 双击 `configure.py` 运行脚本，根据提示填写相关信息。
3. 打开解决方案文件 `EasyPR.sln`，直接编译运行即可。

**方法二**

* 打开解决方案文件 `EasyPR.sln`。

**Note**: 该解决方案会加载两个项目，一个是`EasyPR`，用于编译src/下的源文件生成静态库`libeasypr.lib`；另一个是`Demo`，用来编译test/下的main.cpp，并链接libeasypr.lib生成可执行程序。

* 配置OpenCV

OpenCV for Windows通常会将使用VS编译好二进制文件放到`opencv\build\`目录下。

解决方案自动加载的两个项目配置不符合你的环境，请依次手动配置：

**demo**

|    配置项   |    值    
|-------------|-----------
| `C/C++`-`附加包含目录` | **[opencv3的include目录]**;$(SolutionDir)include
| `链接器`-`附加库目录` | **[opencv3的lib目录]**
| `链接器`-`输入`-`附加依赖项` | **opencv_world300d.lib**;%(AdditionalDependencies)

*Note*：Debug版本为**opencv_world300d.lib**，Release版本为**opencv_world300.lib**

**libeasypr**

|    配置项   |    值    
|-------------|-----------
| `C/C++`-`附加包含目录` | **[opencv3的include目录]**;$(SolutionDir)include


* 生成解决方案

默认情况下，生成的 `demo.exe` 会放在项目根目录下。

**Note**: 直接双击运行程序会出现找不到opencv动态库的情况，这个时候有两个办法：

* 在`opencv3\build\x86(x64)\vc(..)\bin`下找到缺失的dll放到执行目录下。
* 将上述bin目录添加到系统环境变量**PATH**中，然后重新运行程序。

**参考**：windows平台下的opencv的手动配置可以参考这份[博客](http://my.phirobot.com/blog/2014-02-opencv_configuration_in_vs.html)。

#### Linux & Mac OS

EasyPR使用CMake在Linux及Mac OS下进行构建，确保系统安装了最新版本的[CMake](http://cmake.org)。

为了避免系统中安装的老版本opencv对编译的影响，需要在 `CMakeLists.txt` 中修改：

    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/usr/local/opt/opencv3")

路径指向opencv3的安装目录，该目录下应该有OpenCV的CMake配置文件。

项目提供了一键编译shell，在项目根目录下执行：

```
$ ./build
```

即可。

-----

**Note**: *你可以直接利用 EasyPR/include 和编译生成的静态库来调用EasyPR提供的函数接口编写自己的程序。*

运行Demo：

```
$ ./demo // 进入菜单交互界面
$ ./demo ? // 查看CLI帮助
```

### 命令行示例

可以向 `demo[.exe]` 传递命令行参数来完成你想要的工作，目前Demo支持5个子命令。对于每个子命令的帮助信息可以传入 `-h` 参数来获取。

**车牌识别**

    # 利用提供的SVM和ANN模型来识别一张图片里面的所有车牌
    
    $ ./demo recognize -p resources/image/plate_recognize.jpg --svm resources/model/svm.xml --ann resources/model/ann.xml
    
    # 或者更简单一些(注意模型路径)
    $ ./demo recognize -p resources/image/plate_recognize.jpg

**SVM训练**

新版本的EasyPR大大简化了SVM训练：

    # 首先准备好车牌图片集合plates/
    #    是车牌的放在plates/has/
    #    不是车牌的放在plates/no/
    #    车牌可从项目resources/train/svm.7z中解压得到。
    
    $ ./demo svm --plates=path/to/your/plates --svm=save/to/svm.xml
    
    # 该命令将70%的车牌作为训练数据，另外30%的车牌作为测试数据，
    # 这个只可在 include/easypr/config.h 修改。
    # 将训练好的模型存放在 save/to/svm.xml。
	
假设你在easypr的主目录下面新建了一个tmp文件夹，并且把svm.7z解压得到的svm文件夹移动到tmp文件夹下面，

则可以执行 $ demo svm --plates=tmp/svm --svm=tmp/svm.xml，生成得到的tmp文件夹下面的svm.xml就是训练好的模型，

替换resources/model/svm.xml就可以达到替换新模型的目的，替换前请先备份原始模型。

**ANN训练**

先准备好字符图片集合，可从项目resources/train/ann.7z中解压得到。

每类字符都存放在以其名称命名的子文件夹中，命名规则请参考 `include/easypr/config.h`。

一切准备就绪后，运行下面这条命令即可：

    $ ./demo ann --chars=path/to/chars --ann=save/to/ann.xml
	
假设你在easypr的主目录下面新建了一个tmp文件夹，并且把ann.7z解压得到的ann文件夹移动到tmp文件夹下面，

则可以执行 $ demo ann --chars=tmp/ann --ann=tmp/ann.xml，生成得到的tmp文件夹下面的svm.xml就是训练好的模型，

替换resources/model/ann.xml就可以达到替换新模型的目的，替换前请先备份原始模型。

### 获取帮助

详细的开发与教程请见[介绍与开发教程](http://www.cnblogs.com/subconscious/p/3979988.html)。

如果你在使用过程中遇到任何问题，请在[这里](https://github.com/liuruoze/EasyPR/issues)告诉我们。

EasyPR讨论QQ群号是：366392603，加前请注明EasyPR学习讨论。

### Contributors

* liuruoze：1.0-1.2版作者

* 海豚嘎嘎(车主之家)：1.3版算法贡献者，提升了车牌定位与字符识别的准确率

* Micooz：1.3-1.4版架构重构，linux与mac支持，opencv3.0支持

* jsxyhelu：deface版本一

* zhoushiwei：deface版本二

* ahccom：新的plateLocate函数

* 阿水：1.3版整合，数据标注等工作

### 鸣谢

taotao1233，邱锦山，唐大侠，jsxyhelu，如果有一天(zhoushiwei)，学习奋斗，袁承志，圣城小石匠，goldriver，Micooz，梦里时光，Rain Wang，任薛纪，ahccom，星夜落尘，海豚嘎嘎(车主之家)，刘超，以及所有对EasyPR贡献数据的热心同学。