#include "easypr/core/plate_detect.h"
#include "easypr/util/util.h"
#include "easypr/core/core_func.h"
#include "easypr/config.h"

namespace easypr {

  CPlateDetect::CPlateDetect() {
    m_plateLocate = new CPlateLocate();
    m_maxPlates = 3;
    m_type = 0;
    m_showDetect = false;
  }

  CPlateDetect::~CPlateDetect() { SAFE_RELEASE(m_plateLocate); }

  int CPlateDetect::plateDetect(Mat src, std::vector<CPlate> &resultVec, int type,
    bool showDetectArea, int img_index) {
    std::vector<CPlate> sobel_Plates;
    sobel_Plates.reserve(16);
    std::vector<CPlate> color_Plates;
    color_Plates.reserve(16);
    std::vector<CPlate> mser_Plates;
    mser_Plates.reserve(16);
    std::vector<CPlate> all_result_Plates;
    all_result_Plates.reserve(64);
#pragma omp parallel sections
    {
#pragma omp section
      {
        if (!type || type & PR_DETECT_SOBEL) {
          m_plateLocate->plateSobelLocate(src, sobel_Plates, img_index);
        }
      }
#pragma omp section
      {
        if (!type || type & PR_DETECT_COLOR) {
          m_plateLocate->plateColorLocate(src, color_Plates, img_index);
        }
      }
#pragma omp section
      {
        if (!type || type & PR_DETECT_CMSER) {
          m_plateLocate->plateMserLocate(src, mser_Plates, img_index);
        }
      }
    }
    for (auto plate : sobel_Plates) {
      plate.setPlateLocateType(SOBEL);
      all_result_Plates.push_back(plate);
    }
    for (auto plate : color_Plates) {
      plate.setPlateLocateType(COLOR);
      all_result_Plates.push_back(plate);
    }
    for (auto plate : mser_Plates) {
      plate.setPlateLocateType(CMSER);
      all_result_Plates.push_back(plate);
    }
    // use nms to judge plate
    PlateJudge::instance()->plateJudgeUsingNMS(all_result_Plates, resultVec, m_maxPlates);

    if (0)
      showDectectResults(src, resultVec, m_maxPlates);
    return 0;
  }

  int CPlateDetect::plateDetect(Mat src, std::vector<CPlate> &resultVec, int img_index) {
    int result = plateDetect(src, resultVec, m_type, false, img_index);
    return result;
  }

  void CPlateDetect::LoadSVM(std::string path) {
    PlateJudge::instance()->LoadModel(path);
  }

}