#ifndef EASYPR_CORE_PLATEDETECT_H_
#define EASYPR_CORE_PLATEDETECT_H_

#include "easypr/core/plate_locate.h"
#include "easypr/core/plate_judge.h"

namespace easypr {

class CPlateDetect {
 public:
  CPlateDetect();

  ~CPlateDetect();

  //! 深度车牌检测，使用颜色与二次Sobel法综合

  int plateDetect(Mat src, std::vector<CPlate> &resultVec,
                  bool showDetectArea = true, int index = 0);

  //! 展示中间的结果

  int showResult(const Mat &result);

  //! 装载SVM模型

  void LoadSVM(std::string s);

  //! 生活模式与工业模式切换

  inline void setPDLifemode(bool param) { m_plateLocate->setLifemode(param); }

  //! 是否开启调试模式

  inline void setPDDebug(bool param) { m_plateLocate->setDebug(param); }

  //! 获取调试模式状态

  inline bool getPDDebug() { return m_plateLocate->getDebug(); }

  //! 设置与读取变量

  inline void setGaussianBlurSize(int param) {
    m_plateLocate->setGaussianBlurSize(param);
  }

  inline int getGaussianBlurSize() const {
    return m_plateLocate->getGaussianBlurSize();
  }

  inline void setMorphSizeWidth(int param) {
    m_plateLocate->setMorphSizeWidth(param);
  }

  inline int getMorphSizeWidth() const {
    return m_plateLocate->getMorphSizeWidth();
  }

  inline void setMorphSizeHeight(int param) {
    m_plateLocate->setMorphSizeHeight(param);
  }

  inline int getMorphSizeHeight() const {
    return m_plateLocate->getMorphSizeHeight();
  }

  inline void setVerifyError(float param) {
    m_plateLocate->setVerifyError(param);
  }

  inline float getVerifyError() const {
    return m_plateLocate->getVerifyError();
  }

  inline void setVerifyAspect(float param) {
    m_plateLocate->setVerifyAspect(param);
  }

  inline float getVerifyAspect() const {
    return m_plateLocate->getVerifyAspect();
  }

  inline void setVerifyMin(int param) { m_plateLocate->setVerifyMin(param); }

  inline void setVerifyMax(int param) { m_plateLocate->setVerifyMax(param); }

  inline void setJudgeAngle(int param) { m_plateLocate->setJudgeAngle(param); }

  inline void setMaxPlates(int param) { m_maxPlates = param; }

  inline int getMaxPlates() const { return m_maxPlates; }

 private:

  //! 设置一幅图中最多有多少车牌

  int m_maxPlates;

  //! 车牌定位

  CPlateLocate* m_plateLocate;
};

}

#endif  // EASYPR_CORE_PLATEDETECT_H_