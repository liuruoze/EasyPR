### Class CPlateDetect

#### Properties
 - (priavte:)
   - [`int m_maxPlates`](#m_maxPlates)
   - [`CPlateLocate* m_plateLocate`](#m_plateLocate)
   - [`int m_type`](#m_type)
   - [`static std::string m_pathSvm`](#m_pathSvm)
   - [`bool m_showDetect`](#m_showDetect)

#### Functions
 - (public:)
   - [`int plateDetect(Mat src, s td::vector<CPlate> &resultVec, int type,bool showDetectArea, int img_index = 0)`](#plateDetect1)
   - [`int plateDetect(Mat src, std::vector<CPlate> &resultVec, int img_index = 0)`](#plateDetect2)
   - [`void LoadSVM(std::string s)`](#LoadSVM)

***
<a id="m_maxPlates"></a>
#### int m_maxPlates(`get|set`)

<a id="m_plateLocate"></a>
#### CPlateLocate* m_plateLocate
 - 用于车牌定位的对象,完成了所有车牌定位的操作

<a id="m_type"></a>
#### m_type(`set`)
 -

***
