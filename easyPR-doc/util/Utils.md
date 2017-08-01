### Class Utils

#### Properties
 -

#### Functions
 - [`static long getTimestamp()`](#getTimestamp)
 - [`static std::string getFileName(const std::string &path,const bool postfix = false)`](#getFileName)
 - [`static std::vector<std::string> splitString(const std::string &str,const char delimiter)`](#splitString)
 - [`static T min(const T &v1, const T &v2)`](#min)
 - [`static std::vector<std::string> getFiles(const std::string &folder,const bool all = true)`](#getFiles)
 - [`static void print_str_lines(const char** lines)`](#print_str_lines1)
 - [`static void print_str_lines(const std::initializer_list<const char*> &lines)`](#print_str_lines2)
 - [`static void print_file_lines(const std::string &file)`](#print_file_lines)
 - [`static unsigned int levenshtein_distance(const T &s1, const T &s2)`](#levenshtein_distance)
 - [`static bool mkdir(const std::string folder)`](#mkdir)
 - [`static bool imwrite(const std::string &file, const cv::Mat &image)`](#imwrite)
 - [`(private)static std::size_t get_last_slash(const std::string &path)`](#get_last_slash)

***

***

<a id="getTimestamp"></a>
#### long getTimestamp()
 - windows:
   - 返回`cv::getTickCount()`,即系统启动到当前的时间
   - `cv::getTickCount()`经常用于`opencv`下计算代码运行时间
 - linux:
   - 使用linux时间结构体`timespes`,`CLOCK_MONOTONIC`为系统启动时间
   - 以毫秒的形式返回系统启动到当前的时间
 - macOS:
   - 使用了1970/1/1来代替计算时间

<a id="getFileName"></a>
#### std::string getFileName(const std::string &path,const bool postfix = false)
 - 输入`string &path`:文件名,`bool postfix`:是否需要后缀
 - 返回文件名(是否有后缀取决于postfix的值)

<a id="splitString"></a>
#### static std::vector< std::string > splitString(const std::string &str,const char delimiter)
 - 输入`string &str`和`const char delimiter`分隔符
 - 返回`vector<std::string>`,存储了用分隔符分割的字符串

<a id="min"></a>
#### static T min(const T &v1, const T &v2)
 - 返回`(v1 < v2) ? v1 : v2`;

<a id="getFiles"></a>
#### static std::vector< std::string > getFiles(const std::string &folder,const bool all = true)
 - 输入`string &folder`:目录,`bool all`:是否搜索子目录
 - 返回`vector<std::string>`:文件的集合

<a id="print_str_lines1"></a>
#### static void print\_str\_lines(const char** lines)
 - 打印行

<a id="print_str_lines2"></a>
#### static void print\_str\_lines(const std::initializer_list<const char*> &lines)
 - 打印行

<a id="print_file_lines"></a>
#### static void print\_file\_lines(const std::string &file)
 - 读取`file`中的内容,并打印

<a id="levenshtein_distance"></a>
#### static unsigned int levenshtein_distance(const T &s1, const T &s2)
 - 输入两个`T`类型`&s1`和`&s2`
 - 返回这两个数据的编辑距离(Levenshtein Distance),即从s1转化到s2需要的最小步数.

<a id="mkdir"></a>
#### static bool mkdir(const std::string folder)
 - 输入`string folder`:需要创建的目录
 - 返回:`bool`,表示创建成功或者失败

<a id="imwrite"></a>
#### static bool imwrite(const std::string &file, const cv::Mat &image)
 - 输入`string &file`:路径,`Mat &image`:图片
 - 输出:是否存储成功

<a id="get_last_slash"></a>
#### std::size\_t get\_last\_slash(const std::string &path)
 - 输入一个`string &path`,代表了路径
 - 返回路径`/`最后的文件名的位置
