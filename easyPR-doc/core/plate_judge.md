### Class PlateJudge

#### Properties
 - (private:)
   - [`static PlateJudge* instance_`](#instance_)
   - [`svmCallback extractFeature`](#extractFeature)
   - [`cv::Ptr<ml::SVM> svm_`](#svm_)

#### Functions
 - (public:)
   - [`static PlateJudge* instance()`](#instance)
   - [`void LoadModel(std::string path)`](#LoadModel)
   - [`int plateJudgeUsingNMS(const std::vector<CPlate>&, std::vector<CPlate>&, int maxPlates = 5)`](#plateJudgeUsingNMS)
   - [`int plateSetScore(CPlate& plate)`](#plateSetScore)
   - [`int plateJudge(const Mat& plateMat)`](#plateJudge1)
   - [`int plateJudge(const std::vector<Mat> &inVec,std::vector<Mat> &resultVec)`](#plateJudge2)
   - [`int plateJudge(const std::vector<CPlate> &inVec,std::vector<CPlate> &resultVec)`](#plateJudge3)
   - [`void NMS(std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, double overlap)`](#NMS)
   - [`int PlateJudge::plateJudgeUsingNMS(const std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, int maxPlates)`](#plateJudgeUsingNMS)
 - (private:)
   - [`PlateJudge()`](#PlateJudge)

***
<a id="instance_"></a>
#### static PlateJudge* instance_
 - 静态变量`instance_`,用于单例模式

***
<a id="instance"></a>
#### static PlateJudge* instance()
 - 单例模式初始化方法,当`instance_`为`nullptr`时,初始化一个`PlateJudge`
 - 返回`instance_`

<a id="PlateJudge"></a>
#### PlateJudge()
 - 加载`svm`的模型,在`config.h`中配置模型路径
 - 可选择是否使用`LBP`模型,因为暂时没有这个模型,所以使用的是`Hist`

<a id="LoadModel"></a>
#### void LoadModel(std::string path)
 - 输入参数为路径`path`
 - 如果`path`与默认`kDefaultSvmPath`路径不同,则加载`path`下的模型,如果相同,那么加载的模型和`kDefaultSvmPath`路径下的相同

<a id="plateSetScore"></a>
#### int plateSetScore(CPlate& plate)
 - 这个函数通过`SVM`为输入的`CPlate`计算分数,小于0且越小为车牌的可能性越大
 - 方法判定`score`大于`0.5`的`CPlate`为不是车牌
 - 返回值为`0`表示是车牌,返回值为`-1`表示不是车牌

<a id="plateJudge1"></a>
#### int plateJudge(const Mat& plateMat)
 - 调用`plateSetScore`得到`0/-1`并返回,`0`代表是车牌,`1`表示不是车牌

<a id="plateJudge2"></a>
#### int plateJudge(const std::vector<Mat> &inVec,std::vector<Mat> &resultVec)
 - 输入为`Mat`的集合`inVec`,代表了需要判断的`Mat`的集合
 - 输出为`resultVec`,代表了所有可能是车牌的集合
 - `return 0`

<a id="plateJudge3"></a>
#### int plateJudge(const std::vector<CPlate> &inVec,std::vector<CPlate> &resultVec)
 - 输入为`CPlate`的集合`inVec`,代表了需要判断的`CPlate`的集合
 - 输出为`resultVec`,代表了所有可能是车牌的集合
 - 和`int plateJudge(const std::vector<Mat> &inVec,std::vector<Mat> &resultVec)`不同的是,在这个方法中会进行一次矩形的缩小再判断
 - `return 0`

<a id="NMS"></a>
#### void NMS(std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, double overlap)

<a id="plateJudgeUsingNMS"></a>
#### int PlateJudge::plateJudgeUsingNMS(const std::vector<CPlate> &inVec, std::vector<CPlate> &resultVec, int maxPlates)
