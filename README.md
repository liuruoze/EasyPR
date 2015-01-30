EasyPR for Linux
======

此版本仅修改了原版本的部分平台相关的代码，可以同时在Windows和Linux下编译运行。原版请查看：

[EasyPR](https://github.com/liuruoze/EasyPR)

# 注意

此版本的源代码全部为`UTF-8(without bom)`编码形式，Windows用户请使用支持`UTF-8`格式的编译器，VS系列的IDE对UTF-8中文支持不理想。

# 编译

## 依赖

* gcc 4.8 或以上
* cmake 2.6 或以上
* opencv 2.4.8 或以上

## 编译opencv

此过程需要较长时间，请耐心等待。

```
$ cd opencv/
$ cmake .
$ make
$ make install
```

注意一定要执行`make install`不然cmake检查EayPR依赖的时候会找不到opencv。

## 编译EasyPR

```
$ cd EasyPR/
$ cmake .
$ make
```

会直接生成使用动态链接库的可执行程序。
