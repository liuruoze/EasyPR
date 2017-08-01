### Class Kv

#### Properties
 - [`std::map<std::string,std::string> data_`](#data)

#### Functions
 - [`Kv()`](#Kv)
 - [`void load(const std::string &file)`](#load)
 - [`std::string get(const std::string &key)`](#get)
 - [`void add(const std::string &key,const std::string &value)`](#add)
 - [`void remove(const std::string &key)`](#remove)
 - [`void clear()`](#clear)

***

<a id="data"></a>
#### std::map< std::string,std::string > data_
 - 存储了`map< string string >`类型的数据

***

<a id="Kv"></a>
#### Kv()
 - 无

<a id="load"></a>
#### void load(const std::string &file)
 - 参数为`file`,即要读入的文件名
 - 解析之前会先将`data_`清空,即调用`this.clear()`
 - 内部解析文件,文件的存储格式都为`string(空格)string\n`
 - 将`space`前的`string`存储在`data_`的第一个`string里`,将空格后的`string`存储在`data_`的第二个`string`里

<a id="get"></a>
#### std::string get(const std::string &key)
 - 参数为在`data_`中的`key`
 - 返回值为`key`对应的`value`
 - 当`value`不存在时,会打印`[Kv] cannot find ${key}`,并返回`""`

<a id="add"></a>
#### void add(const std::string &key,const std::string &value)
 - 参数为`string key`和`string value`
 - 在`data_`中添加`key:value`
 - 如果在`data_`中已经存在`key`时,打印`[Kv] find duplicate: %s = %s , ignore\n`

<a id="remove"></a>
#### void remove(const std::string &key)
 - 参数为`string key`
 - 删除`data_`中`key`为`string key`的值
 - 如果没有这个`key`,打印`[Kv] cannot find ${key}`

<a id="clear"></a>
#### void clear()
 - 执行`data_.clear()`,清空`data_`
