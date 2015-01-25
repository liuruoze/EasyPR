EasyPR版本更新
======

目前EasyPR的版本是1.1，相比上一个版本1.0，有以下更新（这次的更新内容较多，为了跟你现有的项目和代码不冲突，请
谨慎选择全部更新，最好新起一个目录试试新增的功能和内容）：

* svm准确率改进。使用rbf核替代liner核，在车牌判断的准确率提升了8个百分点。

![svm准确率改进](doc/res/svm_upgragde.png)

* 增加两个特征提取方法，并提供了通用的回调函数接口供开发者建立新的特征。

![fetures](doc/res/fetures.png)

* 增加了Debug模式，可以在image/tmp文件夹下看到所有输出的中间图片。

![Debug模式](doc/res/debug.jpg)

* 增加了LifeMode模式，相比默认模式，更适合在生活场景下定位车牌。

![LifeMode模式](doc/res/lifemode.jpg)

* 增加了批量测试功能，这个功能可供测试EasyPR在多幅图片上的整体表现效果。

![批量测试](doc/res/batch_operation.jpg)

* 引入了GDTS(General Data Test Set，通用数据测试集)这个概念，作为EasyPR准确率的评测数据集。

![GDTS](doc/res/general_test.jpg)

* 为了配合GDTS的数据仅用于正常途径(例如学习和研究)，引入了新的[GDSL协议](images/GDSL.txt)。

![GDSL](doc/res/gdsl.jpg)

* 模块化与完善了SVM训练的功能。提供了一个方便简单的训练窗口。这些增加的功能是为了配合即将推出的SVM开发详解这篇文章。

![SVM训练](doc/res/svm_train.jpg)

* 完善了SVM训练的功能。引入了三个数据集的概念，即learn data，train data，test data。

![SVM训练集](doc/res/svm_data.jpg)

* 使用了Precise，Recall，FSocre三个指标作为SVM模型准确率评判的参数与改善的依据。

![SVM指标](doc/res/svm_rate.jpg)

* 引入了levenshtein距离作为EasyPR整体识别准确率误差的评判参数与改善依据。

![levenshtein](doc/res/levenshtein.jpg)

* SVM训练数据中增加了数百张新数据(未经直方图均衡化的车牌图片和非车牌图片)

请自行解压train/data/plate_detect_svm/learn下的压缩文件查看相关信息。

* 增加了一个命令行窗口，作为测试与训练的辅助工具。

注意：上一个版本中image文件下的test.jpg如果有同学下载了，请删除。它的格式不符合新的GDSL协议的约定。
如果想使用测试图片，可以使用1.1版本中新增的test.jpg替代。
