EasyPR
======

EasyPR是一个中文的开源车牌识别系统。其目标是成为一个简单的，灵活的，准确的免费车牌识别引擎。



Features
-------

相比于其他的车牌识别系统，EasyPR有如下特点：
* 它基于openCV这个开源库，这意味着所有它的代码都可以轻易的获取。
* 它能够识别中文，这是它的一个重要特点。
* 它的识别率还可以，考虑到它还有很多待改进和优化的空间，这已经很不错了。



Installation
-----------

EasyPR不需要安装，开发者直接在其上做改动。如果想使用DLL形式引用或者使用C#与Java调用，则可以在[EasyPR_DLL](https://github.com/liuruoze/EasyPR_Dll)中找到。



Usage
-----

使用Git克隆一份拷贝到你本机或者直接下载zip压缩吧。使用vs2010或以上版本的IDE打开sln文件后，就可以看到本项目的所有目录。包含include文件夹下的所有头文件和cpp文件夹下的src文件。

以下表格是本工程中所有文件的解释与关系:

|文件 | 解释
|------|----------
| plate_locate |  车牌定位
| plate_judge | 车牌判断
| plate_detect | 车牌检测，是车牌定位与车牌判断功能的组合
| chars_segment | `ol`, `ul`, `li`, `dl`, `dt`, `dd`
| chars_identify | `table`, `thead`, `tbody`, `tfoot`, `tr`, `td`, `th`
| chars_recognise | `br`, `hr`
| plate_recognize | `ruby`, `rt`, `rp`
| prep.h | `ruby`, `rt`, `rp`
| test.cpp | `ruby`, `rt`, `rp`
