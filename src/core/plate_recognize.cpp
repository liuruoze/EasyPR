#include "easypr/core/plate_recognize.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

CPlateRecognize::CPlateRecognize() {
  // cout << "CPlateRecognize" << endl;
  // m_plateDetect= new CPlateDetect();
  // m_charsRecognise = new CCharsRecognise();
}

// !车牌识别模块
int CPlateRecognize::plateRecognize(Mat src, std::vector<string> &licenseVec) {
  // 车牌方块集合
  vector<CPlate> plateVec;

  // 进行深度定位，使用颜色信息与二次Sobel
  int resultPD = plateDetect(src, plateVec, getPDDebug(), 0);

  if (resultPD == 0) {
    int num = plateVec.size();
    int index = 0;

    //依次识别每个车牌内的符号
    for (int j = 0; j < num; j++) {
      CPlate item = plateVec[j];
      Mat plate = item.getPlateMat();

      //获取车牌颜色
      string plateType = getPlateColor(plate);

      //获取车牌号
      string plateIdentify = "";
      int resultCR = charsRecognise(plate, plateIdentify);
      if (resultCR == 0) {
        string license = plateType + ":" + plateIdentify;
        licenseVec.push_back(license);
      }
    }
    //完整识别过程到此结束

    //如果是Debug模式，则还需要将定位的图片显示在原图左上角
    if (getPDDebug() == true) {
      Mat result;
      src.copyTo(result);

      for (int j = 0; j < num; j++) {
        CPlate item = plateVec[j];
        Mat plate = item.getPlateMat();

        int height = 36;
        int width = 136;
        if (height * index + height < result.rows) {
          Mat imageRoi = result(Rect(0, 0 + height * index, width, height));
          addWeighted(imageRoi, 0, plate, 1, 0, imageRoi);
        }
        index++;

        RotatedRect minRect = item.getPlatePos();
        Point2f rect_points[4];
        minRect.points(rect_points);

        Scalar lineColor = Scalar(255, 255, 255);

        if (item.getPlateLocateType() == SOBEL) lineColor = Scalar(255, 0, 0);

        if (item.getPlateLocateType() == COLOR) lineColor = Scalar(0, 255, 0);

        for (int j = 0; j < 4; j++)
          line(result, rect_points[j], rect_points[(j + 1) % 4], lineColor, 2,
               8);
      }

      //显示定位框的图片
      showResult(result);
    }
  }

  return resultPD;
}

} /*! \namespace easypr*/
