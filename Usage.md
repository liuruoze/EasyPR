# 使用

### 获取源代码

可以访问我们的项目主页获取代码：

* [github](https://github.com/liuruoze/EasyPR)
* [oschina](https://git.oschina.net/easypr/EasyPR)

如果你熟悉git版本控制工具，可以使用下面的命令从 Github 克隆代码：

    $ git clone https://github.com/liuruoze/EasyPR

### 准备工作

1. 安装OpenCV

从 [OpenCV官方网站](http://opencv.org/) 获取对应平台的 `OpenCV3.1.0`。

EasyPR支持当前主流的操作系统，通常不需要对源代码进行更改就可以编译运行，尽管如此，不同平台上IDE的配置也是有很大差异的，下面主要说明Windows，Linux以及Mac OS下的编译方法。

#### Windows

对于使用 Windows 的同学，建议的IDE是Visual Studio，如果使用的不是VS，请自行配置，也欢迎分享你的配置方案。

|环境 | 需求
|------|----------
| 操作系统 |  64位，Windows7 sp1 / Windows 8 及以上
| Visual Studio | vs2013 / vs2015
| Python（可选） | Python3.*

**Note**: 预编译的 **OpenCV3.1** 已经不支持较低版本的Visual Studio，请选择vs2013及以上的版本。

**方法一（推荐）**

1. 确保将Python的安装目录添加到系统环境变量PATH中。
2. 执行 `python configure.py` ，根据提示填写相关信息。
3. 打开解决方案文件 `EasyPR.sln`，直接编译运行即可。

**方法二**

打开解决方案文件 `EasyPR.sln`，该解决方案会加载两个项目:

* `EasyPR`，用于编译src/下的源文件生成静态库`libeasypr.lib`
* `Demo`，用来编译test/下的main.cpp，并链接libeasypr.lib生成可执行程序。

**配置OpenCV**

OpenCV for Windows 通常会将使用VS编译好二进制文件放到 `opencv\build\` 目录下。

解决方案自动加载的两个项目配置不符合你的环境，请依次手动配置：

**demo**

|    配置项   |    值    
|-------------|-----------
| `C/C++`-`附加包含目录` | **[opencv的include目录]**;$(SolutionDir)include
| `链接器`-`附加库目录` | **[opencv的lib目录]**
| `链接器`-`输入`-`附加依赖项` | **opencv_world310d.lib**;%(AdditionalDependencies)

*Note*：Debug版本为**opencv_world310d.lib**，Release版本为**opencv_world310.lib**

**libeasypr**

|    配置项   |    值    
|-------------|-----------
| `C/C++`-`附加包含目录` | **[opencv的include目录]**;$(SolutionDir)include

**编译**

默认情况下，生成的 `demo.exe` 会放在项目根目录下。

**Note**: 直接双击运行程序会出现找不到opencv动态库的情况，这个时候有两个办法：

* 在`opencv\build\x64\vc(..)\bin`下找到缺失的dll放到执行目录下。
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

----

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

每类字符都存放在以其名称命名的子文件夹中，命名规则请参考  `etc/province_mapping`。

一切准备就绪后，运行下面这条命令即可：

    $ ./demo ann --chars=path/to/chars --ann=save/to/ann.xml
	
假设你在easypr的主目录下面新建了一个tmp文件夹，并且把ann.7z解压得到的ann文件夹移动到tmp文件夹下面，

则可以执行 $ demo ann --chars=tmp/ann --ann=tmp/ann.xml，生成得到的tmp文件夹下面的svm.xml就是训练好的模型，

替换resources/model/ann.xml就可以达到替换新模型的目的，替换前请先备份原始模型。