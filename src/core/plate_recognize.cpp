#include "easypr/core/plate_recognize.h"
#include "easypr/config.h"

namespace easypr {

CPlateRecognize::CPlateRecognize() { }

// !杞︾墝璇嗗埆妯″潡
int CPlateRecognize::plateRecognize(Mat src, std::vector<CPlate> &licenseVec, int index) {
  // 车牌方块集合
  vector<CPlate> plateVec;

  // 进行深度定位，使用颜色信息与二次Sobel
  int resultPD = plateDetect(src, plateVec, getPDDebug(), index);

  if (resultPD == 0) {
    int num = plateVec.size();
    int i = 0;

    //依次识别每个车牌内的符号
    for (int j = 0; j < num; j++) {
      CPlate item = plateVec[j];
      Mat plate = item.getPlateMat();

      //获取车牌颜色
      string plateType = getPlateColor(plate);

      //获取车牌号
      string plateIdentify = "";
      int resultCR = charsRecognise(plate, plateIdentify, index);
      if (resultCR == 0) {
        string license = plateType + ":" + plateIdentify;
        item.setPlateStr(license);
        licenseVec.push_back(item);
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
        if (height * i + height < result.rows) {
          Mat imageRoi = result(Rect(0, 0 + height * i, width, height));
          addWeighted(imageRoi, 0, plate, 1, 0, imageRoi);
        }
        i++;

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


// !车牌识别模块

int CPlateRecognize::plateRecognize(Mat src,
                                    std::vector<std::string> &licenseVec) {

  // 杞︾墝鏂瑰潡闆嗗悎

  std::vector<CPlate> plateVec;

  // 杩涜娣卞害瀹氫綅锛屼娇鐢ㄩ鑹蹭俊鎭笌浜屾Sobel

  int resultPD = plateDetect(src, plateVec, kDebug, 0);

  if (resultPD == 0) {
    size_t num = plateVec.size();
    int index = 0;

    //渚濇璇嗗埆姣忎釜杞︾墝鍐呯殑绗﹀彿

    for (size_t j = 0; j < num; j++) {
      CPlate item = plateVec[j];
      Mat plate = item.getPlateMat();

      //鑾峰彇杞︾墝棰滆壊

      std::string plateType = getPlateColor(plate);

      //鑾峰彇杞︾墝鍙

      std::string plateIdentify = "";
      int resultCR = charsRecognise(plate, plateIdentify);
      if (resultCR == 0) {
        std::string license = plateType + ":" + plateIdentify;
        licenseVec.push_back(license);
      }
    }

    //瀹屾暣璇嗗埆杩囩▼鍒版缁撴潫

    //濡傛灉鏄疍ebug妯″紡锛屽垯杩橀渶瑕佸皢瀹氫綅鐨勫浘鐗囨樉绀哄湪鍘熷浘宸︿笂瑙

    if (getPDDebug()) {
      Mat result;
      src.copyTo(result);

      for (size_t j = 0; j < num; j++) {
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

      //鏄剧ず瀹氫綅妗嗙殑鍥剧墖

      showResult(result);
    }
  }

  return resultPD;
}
}
