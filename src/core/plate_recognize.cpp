#include "easypr/plate_recognize.h"

/*! \namespace easypr
    Namespace where all the C++ EasyPR functionality resides
*/
namespace easypr {

using namespace cv;
using namespace std;

CPlateRecognize::CPlateRecognize() {

}

int CPlateRecognize::plateRecognize(Mat src, vector<string>& licenseVec) {
  // 车牌方块集合
  vector<CPlate> plateVec;

  // 如果设置了Debug模式，就依次显示所有的图片
  // int showDetectArea = getPDDebug();
  bool showDetectArea = false;
  // 进行深度定位，使用颜色信息与二次Sobel
  int resultPD = plateDetectDeep(src, plateVec, showDetectArea, 0);

  Mat result;
  src.copyTo(result);

  if (resultPD == 0) {
    size_t num = plateVec.size();

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

        RotatedRect minRect = item.getPlatePos();
        Point2f rect_points[4];
        minRect.points(rect_points);

        if (item.bColored) {
          for (int k = 0; k < 4; k++) {
            line(result, rect_points[k], rect_points[(k + 1) % 4],
                 Scalar(255, 255, 0), 2, 8);
            //颜色定位车牌，黄色方框
          }
        } else {
          for (int m = 0; m < 4; m++) {
            line(result, rect_points[m], rect_points[(m + 1) % 4],
                 Scalar(0, 0, 255), 2, 8);//sobel定位车牌，红色方框
          }
        }
      }
    }
  }
  showResult(result);

  return resultPD;
}

}  /*! \namespace easypr*/

