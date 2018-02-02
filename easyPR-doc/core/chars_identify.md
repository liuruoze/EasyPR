### Class CharsIdentify

#### Properties
 - (private:)
   - [`annCallback extractFeature`](#extractFeature)
   - [`static CharsIdentify* instance_`](#instance_)
   - [`cv::Ptr<cv::ml::ANN_MLP> ann_`](#ann_)
   - [`cv::Ptr<cv::ml::ANN_MLP> annChinese_`](#annChinese_)
   - [`cv::Ptr<cv::ml::ANN_MLP> annGray_`](#annGray_)
   - [`std::shared_ptr<Kv> kv_`](#kv_)

#### Functions
 - (public:)
   - [`static CharsIdentify* instance()`](#instance)
   - [`int classify(cv::Mat f, float& maxVal, bool isChinses = false, bool isAlphabet = false)`](#classify1)
   - [`void classify(cv::Mat featureRows, std::vector<int>& out_maxIndexs,std::vector<float>& out_maxVals, std::vector<bool> isChineseVec)`](#classify2)
   - [`void classify(std::vector<CCharacter>& charVec)`](#classify3)
   - [`void classifyChinese(std::vector<CCharacter>& charVec)`](#classifyChinese)
   - [`void classifyChineseGray(std::vector<CCharacter>& charVec)`](#classifyChineseGray)
   - [`std::pair<std::string, std::string> identify(cv::Mat input, bool isChinese = false, bool isAlphabet = false)`](#identify1)
   - [`int identify(std::vector<cv::Mat> inputs, std::vector<std::pair<std::string, std::string>>& outputs,std::vector<bool> isChineseVec)`](#identify2)
   - [`std::pair<std::string, std::string> identifyChinese(cv::Mat input, float& result, bool& isChinese)`](#identifyChinese)
   - [`std::pair<std::string, std::string> identifyChineseGray(cv::Mat input, float& result, bool& isChinese)`](#identifyChineseGray)
   - [`bool isCharacter(cv::Mat input, std::string& label, float& maxVal, bool isChinese = false)`](#isCharacter)
   - [`void LoadModel(std::string path)`](#LoadModel)
   - [`void LoadChineseModel(std::string path)`](#LoadChineseModel)
   - [`void LoadGrayChANN(std::string path)`](#LoadGrayChANN)
   - [`void LoadChineseMapping(std::string path)`](#LoadChineseMapping)
 - (private:)
   - [`CharsIdentify()`](#CharsIdentify)

***

***
