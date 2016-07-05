#ifndef EASYPR_CORE_PLATEDETECT_H_
#define EASYPR_CORE_PLATEDETECT_H_

#include "easypr/core/plate_locate.h"
#include "easypr/core/plate_judge.h"

namespace easypr {

class CPlateDetect {
 public:
  CPlateDetect();

  ~CPlateDetect();

  /** @brief Plate detect in an image.

  The function detects plate in an image. It can use sobel, color, and character method or the combinations of them.

  @param src Source image. 
  @param resultVec Destination vector of CPlate.
  @param type Detect type. (eg. PR_DETECT_SOBEL + PR_DETECT_COLOR)
  @param showDetectArea 
  @param index
  */
  int plateDetect(Mat src, std::vector<CPlate> &resultVec, int type,
                  bool showDetectArea, int img_index = 0);


  /** @brief Plate detect in an image.

  The function detects plate in an image. It can use sobel, color, and character method or the combinations of them. 
  Use default m_type, it can use setDetectType() to set it;

  @param src Source image.
  @param resultVec Destination vector of CPlate.
  @param index
  */
  int plateDetect(Mat src, std::vector<CPlate> &resultVec, int img_index = 0);

  Mat showResult(const Mat &result, int img_index = 0);

  void LoadSVM(std::string s);

  inline void setPDLifemode(bool param) { m_plateLocate->setLifemode(param); }

  inline void setPDDebug(bool param) { 
    m_plateLocate->setDebug(param); 
    setDetectShow(param);
  }

  inline bool getPDDebug() { return m_plateLocate->getDebug(); }

  inline void setDetectType(int param) { m_type = param; }

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

  inline void setDetectShow(bool param) { m_showDetect = param; }
  inline bool getDetectShow() const { return m_showDetect; }

 private:

  int m_maxPlates;

  CPlateLocate* m_plateLocate;

  int m_type;

  static std::string m_pathSvm;

  // show the detect result image
  bool m_showDetect;

};

}

#endif  // EASYPR_CORE_PLATEDETECT_H_