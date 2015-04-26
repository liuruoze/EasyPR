#include "../include/plate_locate.h"

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr{
    
    const float DEFAULT_ERROR = 0.6;//0.6
    const float DEFAULT_ASPECT = 3.75; //3.75
    
    CPlateLocate::CPlateLocate()
    {
        //cout << "CPlateLocate" << endl;
        m_GaussianBlurSize = DEFAULT_GAUSSIANBLUR_SIZE;
        m_MorphSizeWidth = DEFAULT_MORPH_SIZE_WIDTH;
        m_MorphSizeHeight = DEFAULT_MORPH_SIZE_HEIGHT;
        
        m_error = DEFAULT_ERROR;
        m_aspect = DEFAULT_ASPECT;
        m_verifyMin = DEFAULT_VERIFY_MIN;
        m_verifyMax = DEFAULT_VERIFY_MAX;
        
        m_angle = DEFAULT_ANGLE;
        
        m_debug = DEFAULT_DEBUG;
        
        m_labItem = NULL;
    }
    
    //! 生活模式与工业模式切换
    //! 如果为真，则设置各项参数为定位生活场景照片（如百度图片）的参数，否则恢复默认值。
    void CPlateLocate::setLifemode(bool param)
    {
        if(param == true)
        {
            setGaussianBlurSize(5);
            setMorphSizeWidth(10);
            setMorphSizeHeight(3);
            setVerifyError(0.75);
            setVerifyAspect(4.0);
            setVerifyMin(1);
            setVerifyMax(200);
        }
        else
        {
            setGaussianBlurSize(DEFAULT_GAUSSIANBLUR_SIZE);
            setMorphSizeWidth(DEFAULT_MORPH_SIZE_WIDTH);
            setMorphSizeHeight(DEFAULT_MORPH_SIZE_HEIGHT);
            setVerifyError(DEFAULT_ERROR);
            setVerifyAspect(DEFAULT_ASPECT);
            setVerifyMin(DEFAULT_VERIFY_MIN);
            setVerifyMax(DEFAULT_VERIFY_MAX);
        }
    }
    
    
    //! 对minAreaRect获得的最小外接矩形，用纵横比进行判断
    bool CPlateLocate::verifySizes(RotatedRect mr)
    {
        float error = m_error;
        //Spain car plate size: 52x11 aspect 4,7272
        //China car plate size: 440mm*140mm，aspect 3.142857
        
        //Real car plate size: 136 * 32, aspect 4
        float aspect = m_aspect;
        
        //Set a min and max area. All other patchs are discarded
        //int min= 1*aspect*1; // minimum area
        //int max= 2000*aspect*2000; // maximum area
        int min = 34 * 8 * m_verifyMin; // minimum area
        int max = 34 * 8 * m_verifyMax; // maximum area
        
        //Get only patchs that match to a respect ratio.
        float rmin= aspect-aspect*error;
        float rmax= aspect+aspect*error;
        
        int area= mr.size.height * mr.size.width;
        float r = (float)mr.size.width / (float)mr.size.height;
        if(r < 1)
            r= (float)mr.size.height / (float)mr.size.width;
        
        if(( area < min || area > max ) || ( r < rmin || r > rmax ))
            return false;
        else
            return true;
    }
    
    //! 显示最终生成的车牌图像，便于判断是否成功进行了旋转。
    Mat CPlateLocate::showResultMat(Mat src, Size rect_size, Point2f center, int index)
    {
        Mat img_crop;
        
        getRectSubPix(src, rect_size, center, img_crop);
        
        if(m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_crop_" << index << ".jpg";
            imwrite(ss.str(), img_crop);
        }
        
        Mat resultResized;
        resultResized.create(HEIGHT, WIDTH, TYPE);
        
        resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);
        
        if(m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_resize_" << index << ".jpg";
            imwrite(ss.str(), resultResized);
        }
        
        return resultResized;
    }
    
    
    // !基于HSV空间的颜色搜索方法
    int CPlateLocate::colorSearch(const Mat& src, const Color r, Mat& out, vector<RotatedRect>& outRects, int index)
    {
        Mat match_grey;
        
        // width值对最终结果影响很大，可以考虑进行多次colorSerch，每次不同的值
        // 另一种解决方案就是在结果输出到SVM之前，进行线与角的再纠正
        const int color_morph_width = 10;
        const int color_morph_height = 2;
        
        // 进行颜色查找
        colorMatch(src, match_grey, r, false);
        
        
        imwrite((string)projectpath+"./image/tmp/match_grey.jpg", match_grey);
        
        
        Mat src_threshold;
        threshold(match_grey, src_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        
        Mat element = getStructuringElement(MORPH_RECT, Size(color_morph_width, color_morph_height));
        morphologyEx(src_threshold, src_threshold, MORPH_CLOSE, element);
        
        
        imwrite((string)projectpath+"./image/tmp/color.jpg", src_threshold);
        
        
        src_threshold.copyTo(out);
        
        // 查找轮廓
        vector< vector< Point> > contours;
        
        // 注意，findContours会改变src_threshold
        // 因此要输出src_threshold必须在这之前使用copyTo方法
        findContours(src_threshold,
                     contours, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        
        vector<vector<Point>>::iterator itc = contours.begin();
        while (itc != contours.end())
        {
            RotatedRect mr = minAreaRect(Mat(*itc));
            
            // 需要进行大小尺寸判断
            if( !verifySizes(mr))
                itc = contours.erase(itc);
            else {
                ++itc;
                outRects.push_back(mr);
            }
        }
        
        return 0;
    }
    
    bool CPlateLocate::sobelJudge(Mat roi)
    {
        //Mat roi_blur;
        //GaussianBlur(roi, roi_blur, Size(m_GaussianBlurSize, m_GaussianBlurSize),
        //	0, 0, BORDER_DEFAULT );
        Mat grad;
        
        int scale = SOBEL_SCALE;
        int delta = SOBEL_DELTA;
        int ddepth = SOBEL_DDEPTH;
        
        Mat roi_grey;
        cvtColor(roi, roi_grey, CV_RGB2GRAY);
        
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;
        
        Sobel(roi_grey, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_x, abs_grad_x);
        
        Sobel(roi_grey, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_y, abs_grad_y);
        
        addWeighted(abs_grad_x, SOBEL_X_WEIGHT, abs_grad_y, SOBEL_Y_WEIGHT, 0, grad);
        
        Mat roi_threshold;
        threshold(grad, roi_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        
        Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight) );
        morphologyEx(roi_threshold, roi_threshold, MORPH_CLOSE, element);
        
        float channels = roi_threshold.channels();
        float nRows = roi_threshold.rows;
        float nCols = roi_threshold.cols;
        
        float percent = float(countNonZero(roi_threshold)) / float(nRows * nCols);
        //cout << "precent:" << percent << endl;
        
        if (percent >= 0.5)
            return true;
        else
            return false;
        
    }
    
    
    //! 字符尺寸验证
    bool CPlateLocate::verifyCharSizes(Mat r)
    {
        //Char sizes 45x90
        float aspect = 45.0f / 90.0f;
        float charAspect = (float)r.cols / (float)r.rows;
        float error = 0.7;
        float minHeight = 10;
        float maxHeight = 35;
        //We have a different aspect ratio for number 1, and it can be ~0.2
        float minAspect = 0.05;
        float maxAspect = aspect + aspect*error;
        //area of pixels
        float area = countNonZero(r);
        //bb area
        float bbArea = r.cols*r.rows;
        //% of pixel in area
        float percPixels = area / bbArea;
        
        if (percPixels <= 1 && charAspect > minAspect && charAspect < maxAspect && r.rows >= minHeight && r.rows < maxHeight)
            return true;
        else
            return false;
    }
    
    
    //! Sobel第一次搜索
    //! 不限制大小和形状，获取的BoundRect进入下一步
    int CPlateLocate::sobelFrtSearch(const Mat& src, vector<Rect_<float>>& outRects)
    {
        Mat src_threshold;
        //soble操作，得到二值图像
        sobelOper(src, src_threshold, m_GaussianBlurSize, m_MorphSizeWidth, m_MorphSizeHeight);
        
        
        imwrite((string)projectpath+"image/tmp/sobelFrtSearch.jpg", src_threshold);
        
        
        vector< vector< Point> > contours;
        findContours(src_threshold,
                     contours, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        
        vector<vector<Point>>::iterator itc = contours.begin();
        
        vector<RotatedRect> first_rects;
        
        while (itc != contours.end())
        {
            RotatedRect mr = minAreaRect(Mat(*itc));
            
            // 需要进行大小尺寸判断
            if (!verifySizes(mr))
                itc = contours.erase(itc);
            else {
                ++itc;
                first_rects.push_back(mr);
            }
        }
        
        for (int i = 0; i < first_rects.size(); i++)
        {
            RotatedRect roi_rect = first_rects[i];
            
            Rect_<float> safeBoundRect;
            if ( !calcSafeRect(roi_rect, src, safeBoundRect) )
                continue;
            
            outRects.push_back(safeBoundRect);
        }
        return 0;
    }
    
    
    
    
    
    
    
    //! Sobel第二次搜索,对断裂的部分进行再次的处理
    //! 对大小和形状做限制，生成参考坐标
    int CPlateLocate::sobelSecSearchPart( Mat& bound, Point2f refpoint, vector<RotatedRect>& outRects)
    {
        Mat bound_threshold;
        
        ////! 第二次参数比一次精细，但针对的是得到的外接矩阵之后的图像，再sobel得到二值图像
        sobelOperT(bound, bound_threshold, 3, 6, 2);
        
        ////二值化去掉两边的边界
        
        //Mat mat_gray;
        //cvtColor(bound,mat_gray,CV_BGR2GRAY);
        
        //bound_threshold = mat_gray.clone();
        ////threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        //int w = mat_gray.cols;
        //int h = mat_gray.rows;
        //Mat tmp = mat_gray(Rect(w*0.15,h*0.2,w*0.6,h*0.6));
        //int threadHoldV = ThresholdOtsu(tmp);
        //threshold(mat_gray, bound_threshold,threadHoldV, 255, CV_THRESH_BINARY);
        
        Mat tempBoundThread = bound_threshold.clone();
        ////
        clearLiuDingOnly(tempBoundThread);
        
        
        int posLeft = 0,posRight = 0;
        if (bFindLeftRightBound(tempBoundThread,posLeft,posRight))
        {
            //找到两个边界后进行连接修补处理
            if (posRight !=0 && posLeft != 0 && posLeft < posRight)
            {
                int posY = bound_threshold.rows*0.5;
                for (int i=posLeft+bound_threshold.rows*0.1;i<posRight-4;i++)
                {
                    bound_threshold.data[posY*bound_threshold.cols+i] = 255;
                }
                
            }
            
            imwrite((string)projectpath+"image/tmp/repaireimg1.jpg",bound_threshold);
            
            //两边的区域不要
            for (int i=0;i<bound_threshold.rows;i++)
            {
                bound_threshold.data[i*bound_threshold.cols+posLeft] = 0;
                bound_threshold.data[i*bound_threshold.cols+posRight] = 0;
            }
            imwrite((string)projectpath+"image/tmp/repaireimg2.jpg",bound_threshold);
        }
        
        
        
        
        vector< vector< Point> > contours;
        findContours(bound_threshold,
                     contours, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        
        vector<vector<Point>>::iterator itc = contours.begin();
        
        vector<RotatedRect> second_rects;
        while (itc != contours.end())
        {
            RotatedRect mr = minAreaRect(Mat(*itc));
            second_rects.push_back(mr);
            ++itc;
        }
        
        for (int i = 0; i < second_rects.size(); i++)
        {
            RotatedRect roi = second_rects[i];
            if (verifySizes(roi))
            {
                Point2f refcenter = roi.center + refpoint;
                Size2f size = roi.size;
                double angle = roi.angle;
                
                RotatedRect refroi(refcenter, size, angle);
                outRects.push_back(refroi);
            }
        }
        
        return 0;
    }
    
    
    //! Sobel第二次搜索
    //! 对大小和形状做限制，生成参考坐标
    int CPlateLocate::sobelSecSearch(Mat& bound, Point2f refpoint, vector<RotatedRect>& outRects)
    {
        Mat bound_threshold;
        
        //! 第二次参数比一次精细，但针对的是得到的外接矩阵之后的图像，再sobel得到二值图像
        sobelOper(bound, bound_threshold, 3, 10, 3);
        
        //Mat tempBoundThread = bound_threshold.clone();
        //////
        //tempBoundThread = clearLiuDing(tempBoundThread);
        
        
        //int posLeft = 0,posRight = 0;
        //if (bFindLeftRightBound2(tempBoundThread,posLeft,posRight))
        //{
        //	//找到两个边界后进行连接修补处理
        //	if (posRight !=0 && posLeft != 0 && posLeft < posRight)
        //	{
        //		int posY = bound_threshold.rows*0.5;
        //		for (int i=posLeft+bound_threshold.rows*0.1;i<posRight-4;i++)
        //		{
        //			bound_threshold.data[posY*bound_threshold.step[0]+i] = 255;
        //		}
        
        //	}
        
        //	imwrite("image/tmp/repaireimg1.jpg",bound_threshold);
        
        //	//两边的区域不要
        //	for (int i=0;i<bound_threshold.rows;i++)
        //	{
        //		bound_threshold.data[i*bound_threshold.step[0]+posLeft] = 0;
        //		bound_threshold.data[i*bound_threshold.step[0]+posRight] = 0;
        //	}
        //	imwrite("image/tmp/repaireimg2.jpg",bound_threshold);
        //}
        
        
        
        imwrite((string)projectpath+"./image/tmp/sobelSecSearch.jpg", bound_threshold);
        
        
        
        vector< vector< Point> > contours;
        findContours(bound_threshold,
                     contours, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        
        vector<vector<Point>>::iterator itc = contours.begin();
        
        vector<RotatedRect> second_rects;
        while (itc != contours.end())
        {
            RotatedRect mr = minAreaRect(Mat(*itc));
            second_rects.push_back(mr);
            ++itc;
        }
        
        for (int i = 0; i < second_rects.size(); i++)
        {
            RotatedRect roi = second_rects[i];
            if (verifySizes(roi))
            {
                Point2f refcenter = roi.center + refpoint;
                Size2f size = roi.size;
                double angle = roi.angle;
                
                RotatedRect refroi(refcenter, size, angle);
                outRects.push_back(refroi);
            }
        }
        
        return 0;
    }
    
    
    //! Sobel运算//对图像分割，腐蚀和膨胀的操作
    //! 输入彩色图像，输出二值化图像
    int CPlateLocate::sobelOper(const Mat& in, Mat& out, int blurSize, int morphW, int morphH)
    {
        Mat mat_blur;
        mat_blur = in.clone();
        GaussianBlur(in, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);
        
        Mat mat_gray;
        if (mat_blur.channels() == 3)
            cvtColor(mat_blur, mat_gray, CV_RGB2GRAY);
        else
            mat_gray = mat_blur;
        //mat_gray = m_bilateral.BilateralFilter(mat_gray,6);
        
        //equalizeHist(mat_gray, mat_gray);
        
        int scale = SOBEL_SCALE;
        int delta = SOBEL_DELTA;
        int ddepth = SOBEL_DDEPTH;
        
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;
        //对X  soble
        Sobel(mat_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_x, abs_grad_x);
        //对Y  soble
        Sobel(mat_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_y, abs_grad_y);
        //在两个权值组合
        Mat grad;
        addWeighted(abs_grad_x, SOBEL_X_WEIGHT, abs_grad_y, SOBEL_Y_WEIGHT, 0, grad);
        //分割
        Mat mat_threshold;
        double otsu_thresh_val = threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        //腐蚀和膨胀
        Mat element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
        morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);
        
        out = mat_threshold;
        //imshow("im",out);
        //waitKey(0);
        return 0;
    }
    
    void DeleteNotArea(Mat& inmat)
    {
        Mat input_grey;
        cvtColor(inmat, input_grey, CV_BGR2GRAY);
        
        
        int w = inmat.cols;
        int h = inmat.rows;
        
        Mat tmpMat = inmat(Rect(w*0.15,h*0.1,w*0.7,h*0.7));
        //判断车牌颜色以此确认threshold方法
        Color plateType = getPlateType(tmpMat, true);
        Mat img_threshold ;
        if (BLUE == plateType)
        {
            img_threshold = input_grey.clone();
            Mat tmp = input_grey(Rect(w*0.15,h*0.15,w*0.7,h*0.7));
            int threadHoldV = ThresholdOtsu(tmp);
            
            threshold(input_grey, img_threshold,threadHoldV, 255, CV_THRESH_BINARY);
            //threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
            
            imwrite((string)projectpath+"image/tmp/inputgray2.jpg",img_threshold);
            
        }
        else if (YELLOW == plateType)
        {
            img_threshold = input_grey.clone();
            Mat tmp = input_grey(Rect(w*0.1,h*0.1,w*0.8,h*0.8));
            int threadHoldV = ThresholdOtsu(tmp);
            
            threshold(input_grey, img_threshold,threadHoldV, 255, CV_THRESH_BINARY_INV);
            
            
            imwrite((string)projectpath+"image/tmp/inputgray2.jpg",img_threshold);
            
            
            //threshold(input_grey, img_threshold, 10, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
        }
        else
            threshold(input_grey, img_threshold, 10, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        
        int posLeft = 0;
        int posRight = 0;
        
        int top = 0;
        int bottom = img_threshold.rows-1;
        clearLiuDing(img_threshold,top,bottom);
        if (bFindLeftRightBound1(img_threshold,posLeft,posRight))
        {
            inmat = inmat(Rect(posLeft,top,w-posLeft,bottom-top));
            
            /*int posY = inmat.rows*0.5*inmat.cols;
             for (int i=posLeft;i<posRight;i++)
             {
             
             inmat.data[posY+i] = 255;
             }
             */
            /*roiRect.x += posLeft;
             roiRect.width -=posLeft;*/
        }
        
    }
    
    //! 抗扭斜处理
    int CPlateLocate::deskew(const Mat& src, const Mat& src_b, vector<RotatedRect>& inRects, vector<CPlate>& outPlates)
    {
        
        for (int i = 0; i < inRects.size(); i++)
        {
            RotatedRect roi_rect = inRects[i];
            
            float r = (float)roi_rect.size.width / (float)roi_rect.size.height;
            float roi_angle = roi_rect.angle;
            
            Size roi_rect_size = roi_rect.size;
            if (r < 1) {
                roi_angle = 90 + roi_angle;
                swap(roi_rect_size.width, roi_rect_size.height);
            }
            //m_angle=60
            if (roi_angle - m_angle < 0 && roi_angle + m_angle > 0)
            {
                Rect_<float> safeBoundRect;
                bool isFormRect = calcSafeRect(roi_rect, src, safeBoundRect);
                if (!isFormRect)
                    continue;
                
                Mat bound_mat = src(safeBoundRect);
                Mat bound_mat_b = src_b(safeBoundRect);
                
                Point2f roi_ref_center = roi_rect.center - safeBoundRect.tl();
                
                Mat deskew_mat;
                if ((roi_angle - 3 < 0 && roi_angle + 3 > 0)  || 90.0 == roi_angle || -90.0 == roi_angle)
                {
                    deskew_mat = bound_mat;
                }
                else
                {
                    // 角度在5到60度之间的，首先需要旋转 rotation
                    Mat rotated_mat;
                    Mat rotated_mat_b;
                    
                    if (!rotation(bound_mat, rotated_mat, roi_rect_size, roi_ref_center, roi_angle))
                        continue;
                    
                    if (!rotation(bound_mat_b, rotated_mat_b, roi_rect_size, roi_ref_center, roi_angle))
                        continue;
                    
                    // 如果图片偏斜，还需要视角转换 affine
                    double roi_slope = 0;
                    //imshow("1roated_mat",rotated_mat);
                    //imshow("rotated_mat_b",rotated_mat_b);
                    if (isdeflection(rotated_mat_b, roi_angle, roi_slope))
                    {/*cout<<"roi_slope的大小"<<roi_slope<<endl;*/
                        //cout << "roi_angle:" << roi_angle << endl;
                        //cout << "roi_slope:" << roi_slope << endl;
                        affine(rotated_mat, deskew_mat, roi_slope);
                    }
                    else
                        deskew_mat = rotated_mat;
                }
                
                Mat plate_mat;
                plate_mat.create(HEIGHT, WIDTH, TYPE);
                
                
                DeleteNotArea(deskew_mat);
                //如果图像大于我们所要求的图像，对图像进行一个大小变更
                if (deskew_mat.cols >= WIDTH || deskew_mat.rows >= HEIGHT)
                    resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_AREA);
                else
                    resize(deskew_mat, plate_mat, plate_mat.size(), 0, 0, INTER_CUBIC);
                
                //if (1)
                //{
                //	imshow("plate_mat", plate_mat);
                //	waitKey(0);
                //	destroyWindow("plate_mat");
                //}
                
                
                CPlate plate;
                plate.setPlatePos(roi_rect);
                plate.setPlateMat(plate_mat);
                outPlates.push_back(plate);
                
            }
        }
        return 0;
    }
    
    //! 旋转操作
    bool CPlateLocate::rotation(Mat& in, Mat& out, const Size rect_size, const Point2f center, const double angle)
    {
        Mat in_large;
        in_large.create(in.rows*1.5, in.cols*1.5, in.type());
        
        int x = in_large.cols / 2 - center.x > 0 ? in_large.cols / 2 - center.x : 0;
        int y = in_large.rows / 2 - center.y > 0 ? in_large.rows / 2 - center.y : 0;
        
        int width = x + in.cols < in_large.cols ? in.cols : in_large.cols - x;
        int height = y + in.rows < in_large.rows ? in.rows : in_large.rows - y;
        
        /*assert(width == in.cols);
         assert(height == in.rows);*/
        
        if (width != in.cols || height != in.rows)
            return false;
        
        Mat imageRoi = in_large(Rect(x, y, width, height));
        addWeighted(imageRoi, 0, in, 1, 0, imageRoi);
        
        Point2f center_diff(in.cols/2, in.rows/2);
        Point2f new_center(in_large.cols / 2, in_large.rows / 2);
        
        Mat rot_mat = getRotationMatrix2D(new_center, angle, 1);
        
        /*imshow("in_copy", in_large);
         waitKey(0);*/
        
        Mat mat_rotated;
        warpAffine(in_large, mat_rotated, rot_mat, Size(in_large.cols, in_large.rows), CV_INTER_CUBIC);
        
        /*imshow("mat_rotated", mat_rotated);
         waitKey(0);*/
        
        Mat img_crop;
        getRectSubPix(mat_rotated, Size(rect_size.width, rect_size.height), new_center, img_crop);
        
        out = img_crop;
        
        /*imshow("img_crop", img_crop);
         waitKey(0);*/
        
        return true;
        
        
    }
    
    
    //! 是否偏斜
    //! 输入二值化图像，输出判断结果
    bool CPlateLocate::isdeflection(const Mat& in, const double angle, double& slope)
    {/*imshow("in",in);
      waitKey(0);*/
        int nRows = in.rows;
        int nCols = in.cols;
        
        assert(in.channels() == 1);
        
        int comp_index[3];
        int len[3];
        
        comp_index[0] = nRows / 4;
        comp_index[1] = nRows / 4 * 2;
        comp_index[2] = nRows / 4 * 3;
        
        const uchar* p;
        
        for (int i = 0; i < 3; i++)
        {
            int index = comp_index[i];
            p = in.ptr<uchar>(index);
            
            int j = 0;
            int value = 0;
            while (0 == value && j < nCols)
                value = int(p[j++]);
            
            len[i] = j;
        }
        
        //cout << "len[0]:" << len[0] << endl;
        //cout << "len[1]:" << len[1] << endl;
        //cout << "len[2]:" << len[2] << endl;
        //len[0]/len[1]/len[2]这三个应该是取车牌边线的值，来计算车牌边线的斜率
        double maxlen = max(len[2], len[0]);
        double minlen = min(len[2], len[0]);
        double difflen = abs(len[2] - len[0]);
        //cout << "nCols:" << nCols << endl;
        
        double PI = 3.14159265;
        //angle是根据水平那根直线的斜率转换过来的角度
        double g = tan(angle * PI / 180.0);
        
        if (maxlen - len[1] > nCols/32 || len[1] - minlen > nCols/32 ) {
            // 如果斜率为正，则底部在下，反之在上
            double slope_can_1 = double(len[2] - len[0]) / double(comp_index[1]);//求直线的斜率
            double slope_can_2 = double(len[1] - len[0]) / double(comp_index[0]);
            double slope_can_3 = double(len[2] - len[1]) / double(comp_index[0]);
            //cout<<"angle:"<<angle<<endl;
            //cout<<"g:"<<g<<endl;
            //cout << "slope_can_1:" << slope_can_1 << endl;
            //cout << "slope_can_2:" << slope_can_2 << endl;
            //cout << "slope_can_3:" << slope_can_3 << endl;
            //if(g>=0)
            slope = abs(slope_can_1 - g) <= abs(slope_can_2 - g) ? slope_can_1 : slope_can_2;
            // else
            //slope = abs(slope_can_1 - g) >= abs(slope_can_2 - g) ? slope_can_1 : slope_can_2;
            /*slope = max(  double(len[2] - len[0]) / double(comp_index[1]),
             double(len[1] - len[0]) / double(comp_index[0]));*/
            
            //cout << "slope:" << slope << endl;
            return true;
        }
        else {
            slope = 0;
        }
        
        return false;
    }
    
    
    //! 扭变操作//通过opencv的仿射变换
    void CPlateLocate::affine(const Mat& in, Mat& out, const double slope)
    {
        //imshow("in", in);
        //waitKey(0);
        //这里的slope是通过判断是否倾斜得出来的倾斜率
        Point2f dstTri[3];
        Point2f plTri[3];
        
        int height = in.rows;//行
        int width = in.cols;//列
        double xiff = abs(slope) * height;
        
        if (slope > 0)
        {
            //右偏型，新起点坐标系在xiff/2位置
            plTri[0] = Point2f(0, 0);
            plTri[1] = Point2f(width - xiff - 1, 0);
            plTri[2] = Point2f(0 + xiff, height - 1);
            
            dstTri[0] = Point2f(xiff / 2, 0);
            dstTri[1] = Point2f(width - 1 - xiff / 2, 0);
            dstTri[2] = Point2f(xiff/2, height - 1);
        }
        else
        {
            //左偏型，新起点坐标系在 -xiff/2位置
            plTri[0] = Point2f(0 + xiff, 0);
            plTri[1] = Point2f(width - 1, 0);
            plTri[2] = Point2f(0, height - 1);
            
            dstTri[0] = Point2f(xiff/2, 0);
            dstTri[1] = Point2f(width - 1 - xiff + xiff/2, 0);
            dstTri[2] = Point2f(xiff/2, height - 1);
        }
        
        /*dstTri[0] = Point2f(0, 0);
         dstTri[1] = Point2f(WIDTH - 1, 0);
         dstTri[2] = Point2f(0, HEIGHT - 1);*/
        
        Mat warp_mat = getAffineTransform(plTri, dstTri);
        
        Mat affine_mat;
        affine_mat.create(height, width, TYPE);
        
        if (in.rows > HEIGHT || in.cols > WIDTH)
            warpAffine(in, affine_mat, warp_mat, affine_mat.size(), CV_INTER_AREA);//仿射变换
        else
            warpAffine(in, affine_mat, warp_mat, affine_mat.size(), CV_INTER_CUBIC);
        
        out = affine_mat;
        
        //imshow("out", out);
        //waitKey(0);
    }
    
    
    //! 计算一个安全的Rect
    //! 如果不存在，返回false
    bool CPlateLocate::calcSafeRect(const RotatedRect& roi_rect, const Mat& src, Rect_<float>& safeBoundRect)
    {
        Rect_<float> boudRect = roi_rect.boundingRect();
        
        // boudRect的左上的x和y有可能小于0
        float tl_x = boudRect.x > 0 ? boudRect.x : 0;
        float tl_y = boudRect.y > 0 ? boudRect.y : 0;
        // boudRect的右下的x和y有可能大于src的范围
        float br_x = boudRect.x + boudRect.width < src.cols ?
        boudRect.x + boudRect.width - 1 : src.cols - 1;
        float br_y = boudRect.y + boudRect.height < src.rows ?
        boudRect.y + boudRect.height - 1 : src.rows - 1;
        
        float roi_width = br_x - tl_x;
        float roi_height = br_y - tl_y;
        
        if (roi_width <= 0 || roi_height <= 0)
            return false;
        
        // 新建一个mat，确保地址不越界，以防mat定位roi时抛异常
        safeBoundRect = Rect_<float>(tl_x, tl_y, roi_width, roi_height);
        
        return true;
    }
    
    
    int CPlateLocate::deskewOld(Mat src, vector<RotatedRect>& inRects,
                                vector<RotatedRect>& outRects, vector<Mat>& outMats, LocateType locateType)
    {
        int k = 1;
        for(int i=0; i< inRects.size(); i++)
        {
            RotatedRect minRect = inRects[i];
            
            if(verifySizes(minRect))
            {
                float r = (float)minRect.size.width / (float)minRect.size.height;
                float angle = minRect.angle;
                cout << "angle:" << angle << endl;
                
                Size rect_size = minRect.size;
                if (r < 1) {
                    angle = 90 + angle;
                    swap(rect_size.width, rect_size.height);
                }
                
                if (angle - m_angle < 0 && angle + m_angle > 0)
                {
                    Rect_<float> boudRect = minRect.boundingRect();
                    
                    // boudRect的左上的x和y有可能小于0
                    float tl_x = boudRect.x > 0 ? boudRect.x : 0;
                    float tl_y = boudRect.y > 0 ? boudRect.y : 0;
                    // boudRect的右上的x和y有可能大于src的范围
                    float br_x = boudRect.x + boudRect.width < src.cols  ?
                    boudRect.x + boudRect.width - 1 : src.cols - 1;
                    float br_y = boudRect.y + boudRect.height < src.rows  ?
                    boudRect.y + boudRect.height - 1 : src.rows - 1;
                    
                    float roi_width = br_x - tl_x;
                    float roi_height = br_y - tl_y;
                    
                    if (roi_width <= 0 || roi_height <= 0)
                        continue;
                    
                    // 新建一个mat，确保地址不越界，以防mat定位roi时抛异常
                    Rect_<float> roiRect = Rect_<float>(tl_x, tl_y, roi_width, roi_height);
                    
                    Mat src_mat = src(roiRect);
                    
                    //imshow("src_mat", src_mat);
                    //waitKey(0);
                    
                    if (locateType == COLOR)
                    {
                        Mat img_crop;
                        if (0.0 == angle || 90.0 == angle || -90.0 == angle || -0.0 == angle)
                        {
                            //  如果角度等于这些值，则不需要旋转，直接就是正矩形
                            //  以免带来旋转与裁剪中的线性插值带来的误差与模糊
                            img_crop = src_mat;
                        }
                        else if (angle - 5 < 0 && angle + 5 > 0)
                        {
                            //  如果角度小于5度，则不必旋转，直接显示
                            //  以免带来旋转与裁剪中的线性插值带来的误差与模糊
                            img_crop = src_mat;
                        }
                        else
                        {
                            //  如果角度在5度到45度之间，则需要旋转
                            
                            //vector<RotatedRect> rects_tmp;
                            //deskewP(src_mat, BLUE, rects_tmp);
                            
                            Point2f newcenter(roiRect.width / 2, roiRect.height / 2 );
                            /*cout << "a:" << angle;*/
                            Mat rotmat = getRotationMatrix2D(newcenter, angle, 1);
                            
                            Mat img_rotated;
                            warpAffine(src_mat, img_rotated, rotmat, src_mat.size(), CV_INTER_CUBIC);
                            
                            /*imshow("img_rotated", img_rotated);
                             waitKey(0);
                             */
                            Mat middle_crop;
                            getRectSubPix(img_rotated, rect_size, newcenter, middle_crop);
                            
                            /*imshow("middle_crop", middle_crop);
                             waitKey(0);*/
                            
                            if (r <= 10)
                            {
                                Point2f srcTri[4];
                                Point2f dstTri[3];
                                
                                Point2f plTri[3];
                                
                                if (angle < 0)
                                {
                                    double PI = 3.14159265;
                                    double g = tan((angle + 90) * PI / 180.0);
                                    
                                    double xdiff = double(middle_crop.rows) * g;
                                    plTri[0] = Point2f(0 + xdiff, 0);
                                    plTri[1] = Point2f(middle_crop.cols - 1, 0);
                                    plTri[2] = Point2f(0 , middle_crop.rows - 1);
                                }
                                else
                                {
                                    double PI = 3.14159265;
                                    double g = tan(abs(angle) * PI / 180.0);
                                    
                                    double xdiff = double(middle_crop.rows) * g;
                                    plTri[0] = Point2f(0, 0);
                                    plTri[1] = Point2f(middle_crop.cols - 1, 0);
                                    plTri[2] = Point2f(0 + xdiff, middle_crop.rows - 1);
                                }
                                
                                dstTri[0] = Point2f(0, 0);
                                dstTri[1] = Point2f(WIDTH - 1, 0);
                                dstTri[2] = Point2f(0, HEIGHT - 1);
                                
                                //
                                Mat warp_mat = getAffineTransform(plTri, dstTri);
                                ////Mat warp_mat = getPerspectiveTransform( srcTri, dstTri );
                                Mat result_crop;
                                warpAffine(middle_crop, result_crop, warp_mat, Size(WIDTH, HEIGHT), CV_INTER_CUBIC);
                                
                                result_crop.copyTo(img_crop);
                                
                                /*imshow("img_crop", img_crop);
                                 waitKey(0);*/
                            }
                            else
                            {
                                img_crop = middle_crop;
                            }
                            
                        }
                        
                        if (sobelJudge(img_crop)) {
                            
                            Mat plate_img;
                            plate_img.create(HEIGHT, WIDTH, TYPE);
                            if (img_crop.cols >= WIDTH || img_crop.rows >= HEIGHT)
                                resize(img_crop, plate_img, plate_img.size(), 0, 0, INTER_AREA);
                            else
                                resize(img_crop, plate_img, plate_img.size(), 0, 0, INTER_CUBIC);
                            
                            /*imshow("plate_img", plate_img);
                             waitKey(0);*/
                            
                            outRects.push_back(minRect);
                            outMats.push_back(plate_img);
                        }
                    }
                    
                    //if (locateType == SOBEL) {
                    //	vector<Mat> resultVec;
                    //	vector<RotatedRect> resultRects;
                    //	sobelFindAgn(src_mat, resultRects, resultVec);
                    
                    //	for (int j = 0; j < resultRects.size(); j++) {
                    //		Point2f origin_center = Point2f(tl_x, tl_y) + resultRects[j].center;
                    //		RotatedRect origin_rect(origin_center, resultRects[j].size, resultRects[j].angle);
                    //		outRects.push_back(origin_rect);
                    //	}
                    
                    //	for (int j = 0; j < resultVec.size(); j++)
                    //	{
                    //		//if (charJudge(resultVec[j]))
                    //		outMats.push_back(resultVec[j]);
                    //	}
                    //}
                    
                }
            }
        }
        
        return 0;
    }
    
    
    // !基于颜色信息的车牌定位
    int CPlateLocate::plateColorLocate(Mat src, vector<CPlate>& candPlates, int index)
    {
        vector<RotatedRect> rects_color_blue;
        vector<RotatedRect> rects_color_yellow;
        vector<CPlate> plates;
        Mat src_b;
        
        // 查找蓝色车牌
        // 查找颜色匹配车牌
        colorSearch(src, BLUE, src_b, rects_color_blue, index);
        // 进行抗扭斜处理
        deskew(src, src_b, rects_color_blue, plates);
        
        // 查找黄色车牌
        colorSearch(src, YELLOW, src_b, rects_color_yellow, index);
        deskew(src, src_b, rects_color_yellow, plates);
        
        for (int i = 0; i< plates.size(); i++)
        {candPlates.push_back(plates[i]);
            //imshow("123",plates[i].getPlateMat());
            //waitKey(0);
        }
        return 0;
    }
    
    //! Sobel运算
    //! 输入彩色图像，输出二值化图像
    int CPlateLocate::sobelOperT(const Mat& in, Mat& out, int blurSize, int morphW, int morphH)
    {
        Mat mat_blur;
        mat_blur = in.clone();
        GaussianBlur(in, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);
        
        
        Mat mat_gray;
        if (mat_blur.channels() == 3)
            cvtColor(mat_blur, mat_gray, CV_BGR2GRAY);
        else
            mat_gray = mat_blur;
        
        //mat_gray = m_bilateral.BilateralFilter(mat_gray,6);
        
        imwrite("./image/tmp/grayblure.jpg",mat_gray);
        
        //equalizeHist(mat_gray, mat_gray);
        
        int scale = SOBEL_SCALE;
        int delta = SOBEL_DELTA;
        int ddepth = SOBEL_DDEPTH;
        
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;
        
        Sobel(mat_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_x, abs_grad_x);
        
        Sobel(mat_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_y, abs_grad_y);
        
        Mat grad;
        addWeighted(abs_grad_x, 1, abs_grad_y, 0, 0, grad);
        
        imwrite("./image/tmp/graygrad.jpg",grad);
        
        Mat mat_threshold;
        double otsu_thresh_val = threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        
        imwrite("./image/tmp/grayBINARY.jpg",mat_threshold);
        
        Mat element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
        morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);
        
        imwrite("./image/tmp/phologyEx.jpg",mat_threshold);
        
        out = mat_threshold;
        
        return 0;
    }
    
    // !基于垂直线条的车牌定位
    int CPlateLocate::plateSobelLocate(Mat src, vector<CPlate>& candPlates, int index)
    {
        vector<RotatedRect> rects_sobel;
        vector<RotatedRect> rects_sobel_sel;
        vector<CPlate> plates;
        
        vector<Rect_<float>> bound_rects;
        
        // Sobel第一次粗略搜索
        sobelFrtSearch(src, bound_rects);
        
        
        
        vector<Rect_<float>> bound_rects_part;
        //对不符合要求的区域进行扩展
        for (int i=0;i<bound_rects.size();i++)
        {
            double fRatio = bound_rects[i].width*1.0/bound_rects[i].height;
            if (fRatio < 3.0 && fRatio > 1.0 && bound_rects[i].height < 120)
            {
                Rect_<float> itemRect = bound_rects[i];
                //宽度过小，进行扩展
                itemRect.x = 	itemRect.x-itemRect.height*(4-fRatio);
                if (itemRect.x < 0)
                {
                    itemRect.x = 0;
                }
                itemRect.width  = itemRect.width+itemRect.height*2*(4-fRatio);
                if (itemRect.width + itemRect.x >= src.cols)
                {
                    itemRect.width = src.cols-itemRect.x;
                }
                
                itemRect.y = itemRect.y-itemRect.height*0.08;
                itemRect.height =itemRect.height*1.16;
                
                bound_rects_part.push_back(itemRect);
            }
            
        }
        //对断裂的部分进行二次处理
        for (int i = 0; i < bound_rects_part.size(); i++)
        {
            Rect_<float> bound_rect = bound_rects_part[i];
            Point2f refpoint(bound_rect.x, bound_rect.y);
            
            int x = bound_rect.x > 0 ? bound_rect.x : 0;
            int y = bound_rect.y > 0 ? bound_rect.y : 0;
            
            int width = x + bound_rect.width < src.cols ?  bound_rect.width : src.cols - x;
            int height = y + bound_rect.height < src.rows ?  bound_rect.height : src.rows - y;
            
            Rect safe_bound_rect(x, y, width, height);
            Mat bound_mat = src(safe_bound_rect);
            
            // Sobel第二次精细搜索
            sobelSecSearchPart(bound_mat, refpoint, rects_sobel);
        }
        
        for (int i = 0; i < bound_rects.size(); i++)
        {
            Rect_<float> bound_rect = bound_rects[i];
            Point2f refpoint(bound_rect.x, bound_rect.y);
            
            int x = bound_rect.x > 0 ? bound_rect.x : 0;
            int y = bound_rect.y > 0 ? bound_rect.y : 0;
            
            int width = x + bound_rect.width < src.cols ?  bound_rect.width : src.cols - x;
            int height = y + bound_rect.height < src.rows ?  bound_rect.height : src.rows - y;
            
            Rect safe_bound_rect(x, y, width, height);
            Mat bound_mat = src(safe_bound_rect);
            
            // Sobel第二次精细搜索
            sobelSecSearch(bound_mat, refpoint, rects_sobel);
            //sobelSecSearchPart(bound_mat, refpoint, rects_sobel);
        }
        
        Mat src_b;
        sobelOper(src, src_b, 3, 10, 3);
        
        // 进行抗扭斜处理
        deskew(src, src_b, rects_sobel, plates);
        
        for (int i = 0; i< plates.size(); i++)
            candPlates.push_back(plates[i]);
        
        return 0;
    }
    
    //				Point2f srcTri[3];
    //				Point2f dstTri[3];
    //
    //				Point2f rect_points[4];
    //				minRect.points( rect_points );
    //
    //				for(int i = 0; i < 4; i++)
    //					rect_points[i] -= bouding.tl();
    //
    //				for(int i = 0; i < 3; i++) {
    //					for(int j = 0; j < 3-i; j++) {
    //						if (rect_points[j].x > rect_points[j+1].x) {
    //							Point2f t = rect_points[j];
    //							rect_points[j] = rect_points[j+1];
    //							rect_points[j+1] = t;
    //						}
    //					}
    //				}
    //
    //				if (rect_points[0].y < rect_points[1].y) {
    //					srcTri[0] = rect_points[0];
    //					srcTri[2] = rect_points[1];
    //				} else {
    //					srcTri[0] = rect_points[1];
    //					srcTri[2] = rect_points[0];
    //				}
    
    
    
    
    //! deprected
    //! 定位车牌图像
    //! src 原始图像
    //! resultVec 一个Mat的向量，存储所有抓取到的图像
    //! 成功返回0，否则返回-1
    //int CPlateLocate::plateLocate(Mat src, vector<Mat>& resultVec, int index)
    //{
    //	Mat src_blur, src_gray;
    //	Mat grad;
    //	Mat src_hist;
    //	Mat src_color;
    //
    //	int scale = SOBEL_SCALE;
    //	int delta = SOBEL_DELTA;
    //	int ddepth = SOBEL_DDEPTH;
    //
    //	if( !src.data )
    //	{ return -1; }
    //
    //	//测试，三通道划分为单通道
    //	//vector<Mat> channels;
    //	//split(src, channels);
    //	//Mat imageBlue = channels.at(0);
    //	//if(1)
    //	//{
    //	//	stringstream ss(stringstream::in | stringstream::out);
    //	//	ss << "image/tmp/debug_imageBlue" << index << ".jpg";
    //	//	imwrite(ss.str(), imageBlue);
    //	//}
    //
    //	//高斯模糊。Size中的数字影响车牌定位的效果。
    //	GaussianBlur( src, src_blur, Size(m_GaussianBlurSize, m_GaussianBlurSize),
    //		0, 0, BORDER_DEFAULT );
    //
    //	if(m_debug)
    //	{
    //		stringstream ss(stringstream::in | stringstream::out);
    //		ss << "image/tmp/debug_GaussianBlur" << ".jpg";
    //		imwrite(ss.str(), src_blur);
    //	}
    //
    //	/// Convert it to gray
    //	cvtColor( src_blur, src_gray, CV_RGB2GRAY );
    //
    //	if(m_debug)
    //	{
    //		stringstream ss(stringstream::in | stringstream::out);
    //		ss << "image/tmp/debug_gray" << ".jpg";
    //		imwrite(ss.str(), src_gray);
    //	}
    //
    //	/// Generate grad_x and grad_y
    //	Mat grad_x, grad_y;
    //	Mat abs_grad_x, abs_grad_y;
    //
    //	/// Gradient X
    //	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    //	Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    //	convertScaleAbs( grad_x, abs_grad_x );
    //
    //	/// Gradient Y
    //	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    //	Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    //	convertScaleAbs( grad_y, abs_grad_y );
    //
    //	/// Total Gradient (approximate)
    //	addWeighted( abs_grad_x, SOBEL_X_WEIGHT, abs_grad_y, SOBEL_Y_WEIGHT, 0, grad );
    //
    //	//Laplacian( src_gray, grad_x, ddepth, 3, scale, delta, BORDER_DEFAULT );
    //	//convertScaleAbs( grad_x, grad );
    //
    //	if(m_debug)
    //	{
    //		stringstream ss(stringstream::in | stringstream::out);
    //		//ss << "image/tmp/debug_Sobel_" << index << ".jpg";
    //		ss << "image/tmp/" << index << "_" << 3 <<"_debug_Sobel" << ".jpg";
    //		imwrite(ss.str(), grad);
    //	}
    //
    //	//if(1)
    //	//{
    //	//	stringstream ss(stringstream::in | stringstream::out);
    //	//	ss << "image/tmp/" << index << "_" << 4 <<"_src_combin" << ".jpg";
    //	//	imwrite(ss.str(), src_combin);
    //	//}
    //
    //	Mat img_threshold;
    //	threshold(grad, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);
    //
    //	if(0)
    //	{
    //		stringstream ss(stringstream::in | stringstream::out);
    //		//ss << "image/tmp/debug_threshold_" << index << ".jpg";
    //		ss << "image/tmp/" << index << "_" << 5 <<"_img_threshold" << ".jpg";
    //		imwrite(ss.str(), img_threshold);
    //	}
    //
    //	Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight) );
    //	morphologyEx(img_threshold, img_threshold, MORPH_CLOSE, element);
    //
    //	if(1)
    //	{
    //		stringstream ss(stringstream::in | stringstream::out);
    //		//ss << "image/tmp/debug_morphology_" << index << ".jpg";
    //		ss << "image/tmp/" << index << "_" << 6 <<"_morph" << ".jpg";
    //		imwrite(ss.str(), img_threshold);
    //	}
    //
    //	//Find 轮廓 of possibles plates
    //	vector< vector< Point> > contours;
    //	findContours(img_threshold,
    //		contours, // a vector of contours
    //		CV_RETR_EXTERNAL, // 提取外部轮廓
    //		CV_CHAIN_APPROX_NONE); // all pixels of each contours
    //
    //	Mat result;
    //	if(1)
    //	{ 
    //		//// Draw blue contours on a white image
    //		src.copyTo(result);
    //
    //		//drawContours(result, contours,
    //		//	-1, // draw all contours
    //		//	Scalar(0,0,255), // in blue
    //		//	1); // with a thickness of 1 
    //
    //		//stringstream ss(stringstream::in | stringstream::out);
    //		//ss << "image/tmp/debug_Contours" << ".jpg";
    //		//imwrite(ss.str(), result);
    //	}
    //
    //
    //	//Start to iterate to each contour founded
    //	vector<vector<Point> >::iterator itc = contours.begin();
    //	
    //	vector<RotatedRect> rects;
    //	//Remove patch that are no inside limits of aspect ratio and area.
    //	int t = 0;
    //	while (itc != contours.end())
    //	{
    //		//Create bounding rect of object
    //		RotatedRect mr = minAreaRect(Mat(*itc));
    //
    //		//large the rect for more
    //		if( !verifySizes(mr))
    //		{
    //			itc = contours.erase(itc);
    //		}
    //		else
    //		{
    //			++itc;
    //			rects.push_back(mr);
    //		}
    //	}
    //
    //	int k = 1;
    //	for(int i=0; i< rects.size(); i++)
    //	{
    //		RotatedRect minRect = rects[i];
    //		if(verifySizes(minRect))
    //		{	
    //			// rotated rectangle drawing 
    //			// Get rotation matrix
    //			// 旋转这部分代码确实可以将某些倾斜的车牌调整正，
    //			// 但是它也会误将更多正的车牌搞成倾斜！所以综合考虑，还是不使用这段代码。
    //			// 2014-08-14,由于新到的一批图片中发现有很多车牌是倾斜的，因此决定再次尝试
    //			// 这段代码。
    //			float r = (float)minRect.size.width / (float)minRect.size.height;
    //			float angle = minRect.angle;
    //			Size rect_size = minRect.size;
    //			if (r < 1)
    //			{
    //				angle = 90 + angle;
    //				swap(rect_size.width, rect_size.height);
    //			}
    //			//如果抓取的方块旋转超过m_angle角度，则不是车牌，放弃处理
    //			if (angle - m_angle < 0 && angle + m_angle > 0)
    //			{
    //				if(1)
    //				{ 
    //					Point2f rect_points[4]; 
    //					minRect.points( rect_points );
    //					for( int j = 0; j < 4; j++ )
    //						line( result, rect_points[j], rect_points[(j+1)%4], Scalar(0,255,255), 1, 8 );
    //				}
    //
    //				//Create and rotate image
    //				Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
    //				Mat img_rotated;
    //
    //				/*if(m_debug)
    //				{ 
    //					stringstream ss(stringstream::in | stringstream::out);
    //					ss << "image/tmp/needRotate" << i << ".jpg";
    //					imwrite(ss.str(), result);
    //				}*/
    //
    //				warpAffine(src, img_rotated, rotmat, src.size(), CV_INTER_CUBIC);
    //
    //				/*if(m_debug)
    //				{ 
    //					stringstream ss(stringstream::in | stringstream::out);
    //					ss << "image/tmp/img_rotated" << i << ".jpg";
    //					imwrite(ss.str(), result);
    //				}*/
    //
    //
    //				//Mat resultMat(img_rotated, minRect);
    //				Mat resultMat;
    //				resultMat = showResultMat(img_rotated, rect_size, minRect.center, k++);
    //
    //				resultVec.push_back(resultMat);
    //			}
    //		}
    //	}
    //
    //	if(1)
    //	{ 
    //		stringstream ss(stringstream::in | stringstream::out);
    //		//ss << "image/tmp/debug_result" << ".jpg";
    //		ss << "image/tmp/" << index << "_" << 9 <<"_result" << ".jpg";
    //		imwrite(ss.str(), result);
    //	}
    //
    //	return 0;
    //}
    
    
    //! 新的定位车牌图像功能
    //! 代码由贡献
    //! 将颜色信息与Sobel信息结合做判断
    //! src 原始图像
    //! resultVec 一个Mat的向量，存储所有抓取到的图像
    //! 成功返回0，否则返回-1
    int CPlateLocate::plateLocate(Mat src, vector<Mat>& resultVec, int index)
    {
        Mat src_blur, src_gray;
        Mat grad;
        
        int scale = SOBEL_SCALE;
        int delta = SOBEL_DELTA;
        int ddepth = SOBEL_DDEPTH;
        
        if (!src.data)
        {
            return -1;
        }
        
        //高斯模糊。Size中的数字影响车牌定位的效果。
        GaussianBlur(src, src_blur, Size(m_GaussianBlurSize, m_GaussianBlurSize),
                     0, 0, BORDER_DEFAULT);
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << "image/tmp/debug_GaussianBlur" << ".jpg";
            imwrite(ss.str(), src_blur);
        }
        
        /// Convert it to gray
        cvtColor(src_blur, src_gray, CV_RGB2GRAY);
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_gray" << ".jpg";
            imwrite(ss.str(), src_gray);
        }
        // RGB颜色初定位
        // http://wenku.baidu.com/view/2329e5d2360cba1aa811da65.html?re=view
        // RGB -> HSV
        //      蓝         黄         白         黑
        //H     200~255     25~55       /           /
        //S     0.4~1       0.4~1       0~0.1       /
        //V     0.3~1       0.3~1       0.9~1       0~0.35
        //cvCvtColor(src,dst,CV_BGR2HSV);
        //其中，src为三通道的，dst也为三通道的，
        //OPENCV 中 H、S、V、顺序分别为3*x+0  3*x+1   3*x+2
        //opencv中的 H分量是 0~180， S分量是0~255， V分量是0~255
        //但是HSV颜色空间却规定的是，H范围0~360，S范围0~1，V范围0~1
        //所以你需要自己转换一下，H*2，S/255, V/255
        
        // 默认蓝色车牌
        cv::Mat tmp;
        cv::cvtColor(src, tmp, CV_BGR2HSV);
        vector<Mat> hsvSplit;
        split(tmp, hsvSplit);
        cv::Mat dst_blue(src.rows, src.cols, CV_8UC1);
        cv::Mat dst_yellow(src.rows, src.cols, CV_8UC1);
        for (int i = 0; i<tmp.rows; i++)
        {
            for (int j = 0; j<tmp.cols; j++)
            {
                int nH = hsvSplit[0].at<uchar>(i, j) * 2;
                float fS = hsvSplit[1].at<uchar>(i, j) / 255.0;
                float fV = hsvSplit[2].at<uchar>(i, j) / 255.0;
                if (nH >= 200 && nH <= 255 && fS >= 0.4 && fS <= 1 && fV >= 0.3 && fV <= 1) // 蓝色
                    dst_blue.at<uchar>(i, j) = 255;
                else
                    dst_blue.at<uchar>(i, j) = 0;
            }
        }
        //imshow("dst_blue",dst_blue);
        Mat element_blue = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
        morphologyEx(dst_blue, dst_blue, MORPH_CLOSE, element_blue);
        //Find 轮廓 of possibles plates
        cv::Mat con_blue = dst_blue.clone();
        vector< vector< Point> > contours_blue;
        findContours(con_blue,
                     contours_blue, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        //Start to iterate to each contour founded
        vector<vector<Point> >::iterator itb = contours_blue.begin();
        
        //Remove patch that are no inside limits of aspect ratio and area.
        int tb = 0;
        vector<cv::Rect> rects_blue;
        while (itb != contours_blue.end())
        {
            //Create bounding rect of object
            RotatedRect mr = minAreaRect(Mat(*itb));
            
            Rect_<float> safeBoundRect;
            if (!calcSafeRect(mr, src, safeBoundRect))
            {
                itb++;
                continue;
            }
            
            //large the rect for more
            if (!verifySizes(mr))
            {
                cv::Mat roi = dst_blue(safeBoundRect);
                roi.setTo(0);
                cv::swap(roi, dst_blue);
            }
            else
            {
                rects_blue.push_back(safeBoundRect);
            }
            ++itb;
        }
        //////////////////////////////////////////////////////////////////////////
        for (int i = 0; i<tmp.rows; i++)
        {
            for (int j = 0; j<tmp.cols; j++)
            {
                int nH = hsvSplit[0].at<uchar>(i, j) * 2;
                float fS = hsvSplit[1].at<uchar>(i, j) / 255.0;
                float fV = hsvSplit[2].at<uchar>(i, j) / 255.0;
                if (nH >= 25 && nH <= 55 && fS >= 0.4 && fS <= 1 && fV >= 0.3 && fV <= 1) // 黄色
                    dst_yellow.at<uchar>(i, j) = 255;
                else
                    dst_yellow.at<uchar>(i, j) = 0;
            }
        }
        
        Mat element_yellow = getStructuringElement(MORPH_ELLIPSE, Size(10, 10));
        morphologyEx(dst_yellow, dst_yellow, MORPH_CLOSE, element_blue);
        //Find 轮廓 of possibles plates
        cv::Mat con_yellow = dst_yellow.clone();
        vector< vector< Point> > contours_yellow;
        findContours(con_yellow,
                     contours_yellow, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        //Start to iterate to each contour founded
        vector<vector<Point> >::iterator ity = contours_yellow.begin();
        
        //Remove patch that are no inside limits of aspect ratio and area.
        tb = 0;
        vector<cv::Rect> rects_yellow;
        while (ity != contours_yellow.end())
        {
            //Create bounding rect of object
            RotatedRect mr = minAreaRect(Mat(*ity));
            
            Rect_<float> safeBoundRect;
            if (!calcSafeRect(mr, src, safeBoundRect))
            {
                ity++;
                continue;
            }
            
            //large the rect for more
            if (!verifySizes(mr))
            {
                cv::Mat roi = dst_yellow(safeBoundRect);
                roi.setTo(0);
                cv::swap(roi, dst_yellow);
            }
            else
            {
                rects_yellow.push_back(safeBoundRect);
            }
            ++ity;
        }
        
        /// Generate grad_x and grad_y
        Mat grad_x, grad_y;
        Mat abs_grad_x, abs_grad_y;
        
        /// Gradient X
        //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
        Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_x, abs_grad_x);
        
        /// Gradient Y
        //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
        Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
        convertScaleAbs(grad_y, abs_grad_y);
        
        /// Total Gradient (approximate)
        addWeighted(abs_grad_x, SOBEL_X_WEIGHT, abs_grad_y, SOBEL_Y_WEIGHT, 0, grad);//计算两个数组加权值的和
        //imshow("grad",grad);
        //Laplacian( src_gray, grad_x, ddepth, 3, scale, delta, BORDER_DEFAULT );  
        //convertScaleAbs( grad_x, grad ); 
        cv::Mat out_blue;
        cv::multiply(grad, dst_blue, out_blue);
        //imshow("out_blue",out_blue);
        cv::Mat out_yellow;
        cv::multiply(grad, dst_yellow, out_yellow);
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_Sobel_blue" << ".jpg";
            imwrite(ss.str(), out_blue);
            ss << (string)projectpath+"image/tmp/debug_Sobel_yellow" << ".jpg";
            imwrite(ss.str(), out_yellow);
        }
        
        Mat img_threshold_blue;
        Mat img_threshold_yellow;
        threshold(out_blue, img_threshold_blue, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        threshold(out_yellow, img_threshold_yellow, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        //threshold(grad, img_threshold, 75, 255, CV_THRESH_BINARY);
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_threshold_blue" << ".jpg";
            imwrite(ss.str(), img_threshold_blue);
            ss << (string)projectpath+"image/tmp/debug_threshold_yellow" << ".jpg";
            imwrite(ss.str(), img_threshold_yellow);
        }
        
        Mat element = getStructuringElement(MORPH_RECT, Size(m_MorphSizeWidth, m_MorphSizeHeight));
        morphologyEx(img_threshold_blue, img_threshold_blue, MORPH_CLOSE, element);
        morphologyEx(img_threshold_yellow, img_threshold_yellow, MORPH_CLOSE, element);
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_morphology_blue" << ".jpg";
            imwrite(ss.str(), img_threshold_blue);
            ss << (string)projectpath+"image/tmp/debug_morphology_yellow" << ".jpg";
            imwrite(ss.str(), img_threshold_yellow);
        }
        
        //Find 轮廓 of possibles plates
        contours_blue.clear();
        findContours(img_threshold_blue,
                     contours_blue, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        contours_yellow.clear();
        findContours(img_threshold_yellow,
                     contours_yellow, // a vector of contours
                     CV_RETR_EXTERNAL, // 提取外部轮廓
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        
        Mat result;
        if (m_debug)
        {
            //// Draw blue contours on a white image
            src.copyTo(result);
            drawContours(result, contours_blue,
                         -1, // draw all contours
                         Scalar(0, 0, 255), // in blue
                         1); // with a thickness of 1
            drawContours(result, contours_yellow,
                         -1, // draw all contours
                         Scalar(0, 0, 255), // in blue
                         1); // with a thickness of 1
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_Contours" << ".jpg";
            imwrite(ss.str(), result);
        }
        
        
        //Start to iterate to each contour founded
        itb = contours_blue.begin();
        
        vector<RotatedRect> rects;
        //Remove patch that are no inside limits of aspect ratio and area.
        int t = 0;
        while (itb != contours_blue.end())
        {
            //Create bounding rect of object
            RotatedRect mr = minAreaRect(Mat(*itb));
            
            //large the rect for more
            if (!verifySizes(mr))
            {
                itb = contours_blue.erase(itb);
            }
            else
            {
                ++itb;
                rects.push_back(mr);
            }
        }
        
        ity = contours_yellow.begin();
        while (ity != contours_yellow.end())
        {
            //Create bounding rect of object
            RotatedRect mr = minAreaRect(Mat(*ity));
            
            //large the rect for more
            if (!verifySizes(mr))
            {
                ity = contours_yellow.erase(ity);
            }
            else
            {
                ++ity;
                rects.push_back(mr);
            }
        }
        int k = 1;
        for (int i = 0; i< rects.size(); i++)
        {
            RotatedRect minRect = rects[i];
            if (verifySizes(minRect))
            {
                // rotated rectangle drawing 
                // Get rotation matrix
                // 旋转这部分代码确实可以将某些倾斜的车牌调整正，
                // 但是它也会误将更多正的车牌搞成倾斜！所以综合考虑，还是不使用这段代码。
                // 2014-08-14,由于新到的一批图片中发现有很多车牌是倾斜的，因此决定再次尝试
                // 这段代码。
                if (m_debug)
                {
                    Point2f rect_points[4];
                    minRect.points(rect_points);
                    for (int j = 0; j < 4; j++)
                        line(result, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 255), 1, 8);
                }
                
                float r = (float)minRect.size.width / (float)minRect.size.height;
                float angle = minRect.angle;
                Size rect_size = minRect.size;
                if (r < 1)
                {
                    angle = 90 + angle;
                    swap(rect_size.width, rect_size.height);
                }
                //如果抓取的方块旋转超过m_angle角度，则不是车牌，放弃处理
                if (angle - m_angle < 0 && angle + m_angle > 0)
                {
                    //Create and rotate image
                    Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
                    Mat img_rotated;
                    warpAffine(src, img_rotated, rotmat, src.size(), CV_INTER_CUBIC);
                    
                    Mat resultMat;
                    resultMat = showResultMat(img_rotated, rect_size, minRect.center, k++);
                    
                    resultVec.push_back(resultMat);
                }
            }
        }
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << (string)projectpath+"image/tmp/debug_result" << ".jpg";
            imwrite(ss.str(), result);
        }
        
        return 0;
    }
    
    
    
    
}	/*! \namespace easypr*/