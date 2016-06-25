#include "easypr/core/plate_detect.h"
#include "easypr/util/util.h"

namespace easypr {

  CPlateDetect::CPlateDetect() {
    m_plateLocate = new CPlateLocate();

    m_maxPlates = 3;
    m_type = 0;
  }

  CPlateDetect::~CPlateDetect() { SAFE_RELEASE(m_plateLocate); }

  int CPlateDetect::plateDetect(Mat src, std::vector<CPlate> &resultVec, int type,
    bool showDetectArea, int index) {

    std::vector<CPlate> color_Plates;
    std::vector<CPlate> sobel_Plates;
    std::vector<CPlate> mser_Plates;

    std::vector<CPlate> all_result_Plates;

    //如果颜色查找找到n个以上（包含n个）的车牌，就不再进行Sobel查找了。

    if ( !type || type & PR_DETECT_SOBEL)
    {
      m_plateLocate->plateSobelLocate(src, sobel_Plates, index);
      std::vector<CPlate>& sobel_result_Plates = sobel_Plates;

      for (size_t i = 0; i < sobel_result_Plates.size(); i++) 
      {
        CPlate plate = sobel_result_Plates[i];
        plate.setPlateLocateType(SOBEL);

        all_result_Plates.push_back(plate);
      }
    }

    if ( !type || type & PR_DETECT_COLOR)
    {

      m_plateLocate->plateColorLocate(src, color_Plates, index);
      std::vector<CPlate>& color_result_Plates = color_Plates;

      for (size_t i = 0; i < color_result_Plates.size(); i++)
      {
        CPlate plate = color_result_Plates[i];

        plate.setPlateLocateType(COLOR);
        all_result_Plates.push_back(plate);
      }
    }

    if ( !type || type & PR_DETECT_CMSER)
    {

      m_plateLocate->plateMserLocate(src, mser_Plates, index);
      std::vector<CPlate>& mser_result_Plates = mser_Plates;

      for (size_t i = 0; i < mser_result_Plates.size(); i++)
      {
        CPlate plate = mser_result_Plates[i];

        plate.setPlateLocateType(CMSER);
        all_result_Plates.push_back(plate);
      }
    }

    // 使用非极大值抑制来判断车牌
    PlateJudge::instance()->plateJudgeUsingNMS(all_result_Plates, resultVec, m_maxPlates);

    if (1) 
    {
      Mat result = src.clone();
      for (size_t i = 0; i < all_result_Plates.size(); i++)
      {
        CPlate plate = all_result_Plates[i];

        Rect_<float> outputRect;
        calcSafeRect(plate.getPlatePos(), src, outputRect);

        if (0)
        {
          cv::rectangle(result, outputRect, Scalar(0, 0, 255));
        }

        if (0)
        {
          std::stringstream ss(std::stringstream::in | std::stringstream::out);
          ss << "resources/image/tmp/plate_" << index << "_" << i << ".jpg";
          imwrite(ss.str(), src(outputRect));
        }
      }

      if (0) 
      {
        imshow("result", result);
        waitKey(0);
        destroyWindow("result");
      }
    }

    return 0;
  }

  int CPlateDetect::plateDetect(Mat src, std::vector<CPlate> &resultVec,
    bool showDetectArea, int index) {
    int result =  plateDetect(src, resultVec, m_type, showDetectArea, index);
    return result;
  }

  int CPlateDetect::showResult(const Mat &result) {
    namedWindow("EasyPR", CV_WINDOW_AUTOSIZE);

    const int RESULTWIDTH = 640;   // 640 930
    const int RESULTHEIGHT = 540;  // 540 710

    Mat img_window;
    img_window.create(RESULTHEIGHT, RESULTWIDTH, CV_8UC3);

    int nRows = result.rows;
    int nCols = result.cols;

    Mat result_resize;
    if (nCols <= img_window.cols && nRows <= img_window.rows) {
      result_resize = result;

    }
    else if (nCols > img_window.cols && nRows <= img_window.rows) {
      float scale = float(img_window.cols) / float(nCols);
      resize(result, result_resize, Size(), scale, scale, CV_INTER_AREA);

    }
    else if (nCols <= img_window.cols && nRows > img_window.rows) {
      float scale = float(img_window.rows) / float(nRows);
      resize(result, result_resize, Size(), scale, scale, CV_INTER_AREA);

    }
    else if (nCols > img_window.cols && nRows > img_window.rows) {
      Mat result_middle;
      float scale = float(img_window.cols) / float(nCols);
      resize(result, result_middle, Size(), scale, scale, CV_INTER_AREA);

      if (result_middle.rows > img_window.rows) {
        float scale = float(img_window.rows) / float(result_middle.rows);
        resize(result_middle, result_resize, Size(), scale, scale, CV_INTER_AREA);

      }
      else {
        result_resize = result_middle;
      }
    }
    else {
      result_resize = result;
    }

    Mat imageRoi = img_window(Rect((RESULTWIDTH - result_resize.cols) / 2,
      (RESULTHEIGHT - result_resize.rows) / 2,
      result_resize.cols, result_resize.rows));
    addWeighted(imageRoi, 0, result_resize, 1, 0, imageRoi);

    imshow("EasyPR", img_window);
    waitKey();

    destroyWindow("EasyPR");

    return 0;
  }
}