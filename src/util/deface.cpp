//
//  main.cpp
//  人脸马赛克
//
//  Created by zhou shiwei on 15/1/20.
//  Copyright (c) 2015年 zhou shiwei. All rights reserved.
//

//  Modified by liu ruoze on 15/1/23
//  deface.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <string>

//	反人脸识别文件
//	在发布数据到general_test数据集里，请先用这里的方法对图像进行处理
//	EasyPR开源项目非常注重保护图片中驾驶人的隐私

#include "../include/plate_recognize.h"
#include "../include/util.h"
#include "../include/features.h"

using namespace std;
using namespace cv;

Mat detectAndMaskFace(Mat& img, CascadeClassifier& cascade,
                       double scale );

//	反人脸识别函数
//	在发布数据到general_test数据集里，请先用这里的方法对图像进行处理
//	EasyPR开源项目非常注重保护图片中驾驶人的隐私
int deface() 
{ 
    CascadeClassifier cascade;
    //string cascadeName="/Users/zhoushiwei/Dropbox/Avatar/SDMLIB/haarcascade_frontalface_alt2.xml";
	string cascadeName="model/haarcascade_frontalface_default.xml";

    if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        return -1;
    }

	/**
   //read file
    string dir = "/Users/zhoushiwei/Pictures/openeyes", filepath;
    DIR *dp;
    struct dirent *dirp;
    struct stat filestat;
    dp = opendir( dir.c_str() );
    while ((dirp=readdir(dp))) {
        filepath=dir+"/" +dirp->d_name;
        //if the file is directory , skip the file
        if (stat(filepath.c_str(),&filestat)) {
            continue;
        }
        if (S_ISDIR(filestat.st_mode)) {
            continue;
        }
        printf("%s\n",filepath.c_str());
        Mat img=imread(filepath);
        if (img.empty()) {
            continue;
        }
        detectAndMaskFace(img, cascade, 1.5);
    }
    **/

    return 0;
}

Mat detectAndMaskFace(Mat& img, CascadeClassifier& cascade, double scale)
{
  
    double t = 0;
    vector<Rect> faces;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );
    cvtColor( img, gray, COLOR_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );
    t = (double)cv::getTickCount();
    cascade.detectMultiScale( smallImg, faces,
                             1.1, 2, 0
                             //|CASCADE_FIND_BIGGEST_OBJECT
                             //|CASCADE_DO_ROUGH_SEARCH
                             |CASCADE_SCALE_IMAGE
                             ,
                             Size(30, 30) );
    for (vector<Rect>::const_iterator r=faces.begin(); r!=faces.end(); r++) {
        cv::Rect facerect=*r;
        Mat roi=img(cv::Rect(facerect.x*scale,facerect.y*scale,facerect.width*scale,facerect.height*scale));
        int W=18;
        int H=18;
        for (int i=W; i<roi.cols; i+=W) {
            for (int j=H; j<roi.rows; j+=H) {
                uchar s=roi.at<uchar>(j-H/2,(i-W/2)*3);
                 uchar s1=roi.at<uchar>(j-H/2,(i-W/2)*3+1);
                 uchar s2=roi.at<uchar>(j-H/2,(i-W/2)*3+2);
                for (int ii=i-W; ii<=i; ii++) {
                    for (int jj=j-H; jj<=j; jj++) {
                        roi.at<uchar>(jj,ii*3+0)=s;
                        roi.at<uchar>(jj,ii*3+1)=s1;
                        roi.at<uchar>(jj,ii*3+2)=s2;
                    }
                }
                
            }
        }
    }
   
	return img;
}
