#include "../include/chars_segment.h"

/*! \namespace easypr
 Namespace where all the C++ EasyPR functionality resides
 */
namespace easypr{
    
    const float DEFAULT_BLUEPERCEMT = 0.3;
    const float	DEFAULT_WHITEPERCEMT = 0.1;
    
    CCharsSegment::CCharsSegment()
    {
        //cout << "CCharsSegment" << endl;
        m_LiuDingSize = DEFAULT_LIUDING_SIZE;
        m_theMatWidth = DEFAULT_MAT_WIDTH;
        
        //！车牌颜色判断参数
        m_ColorThreshold = DEFAULT_COLORTHRESHOLD;
        m_BluePercent = DEFAULT_BLUEPERCEMT;
        m_WhitePercent = DEFAULT_WHITEPERCEMT;
        
        m_debug = DEFAULT_DEBUG;
    }
    
    //! 字符尺寸验证
    bool CCharsSegment::verifyCharSizes(Mat r){
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
    
    //! 字符预处理
    Mat CCharsSegment::preprocessChar(Mat in){
        //Remap image
        int h = in.rows;
        int w = in.cols;
        int charSize = CHAR_SIZE;	//统一每个字符的大小
        Mat transformMat = Mat::eye(2, 3, CV_32F);
        int m = max(w, h);
        transformMat.at<float>(0, 2) = m / 2 - w / 2;
        transformMat.at<float>(1, 2) = m / 2 - h / 2;
        
        Mat warpImage(m, m, in.type());
        warpAffine(in, warpImage, transformMat, warpImage.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(0));
        
        Mat out;
        resize(warpImage, out, Size(charSize, charSize));
        
        return out;
    }
    
    
    // implementation of otsu algorithm
    // author: onezeros(@yahoo.cn)
    // reference: Rafael C. Gonzalez. Digital Image Processing Using MATLAB
    
    int staticIndex = 0;
    
    int iTag = 0;
    //! 字符分割与排序
    int CCharsSegment::charsSegment(Mat input, vector<Mat>& resultVec)
    {
        if (!input.data)
            return -3;
        
        int w = input.cols;
        int h = input.rows;
        
        Mat tmpMat = input(cv::Rect(w*0.1,h*0.1,w*0.8,h*0.8));
        //判断车牌颜色以此确认threshold方法
        Color plateType = getPlateType(tmpMat, true);
        
        Mat input_grey;
        cvtColor(input, input_grey, CV_BGR2GRAY);
        
        
        Mat img_threshold ;
        if (BLUE == plateType)
        {
            img_threshold = input_grey.clone();
            
            int w = input_grey.cols;
            int h = input_grey.rows;
            Mat tmp = input_grey(cv::Rect(w*0.1,h*0.1,w*0.8,h*0.8));
            int threadHoldV = ThresholdOtsu(tmp);
            imwrite("./image/tmp/inputgray2.jpg",input_grey);
            
            threshold(input_grey, img_threshold,threadHoldV, 255, CV_THRESH_BINARY);
            
            
            //threshold(input_grey, img_threshold, 5, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
            
        }
        else if (YELLOW == plateType)
        {
            img_threshold = input_grey.clone();
            int w = input_grey.cols;
            int h = input_grey.rows;
            Mat tmp = input_grey(cv::Rect(w*0.1,h*0.1,w*0.8,h*0.8));
            int threadHoldV = ThresholdOtsu(tmp);
            imwrite("./image/tmp/inputgray2.jpg",input_grey);
            
            threshold(input_grey, img_threshold,threadHoldV, 255, CV_THRESH_BINARY_INV);
            
            
            //threshold(input_grey, img_threshold, 10, 255, CV_THRESH_OTSU + CV_THRESH_BINARY_INV);
        }
        else
            threshold(input_grey, img_threshold, 10, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
        
        
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << "image/tmp/debug_char_threshold" <<iTag<< ".jpg";
            imwrite(ss.str(), img_threshold);
        }
        
        //去除车牌上方的柳钉以及下方的横线等干扰
        if(!clearLiuDing(img_threshold))
        {
            return -3;
        }
        
        if (m_debug)
        {
            stringstream ss(stringstream::in | stringstream::out);
            ss << "image/tmp/debug_char_clearLiuDing" <<iTag<< ".jpg";
            imwrite(ss.str(), img_threshold);
        }
        iTag++;
        
        Mat img_contours;
        img_threshold.copyTo(img_contours);
        
        vector< vector< Point> > contours;
        findContours(img_contours,
                     contours, // a vector of contours
                     CV_RETR_EXTERNAL, // retrieve the external contours
                     CV_CHAIN_APPROX_NONE); // all pixels of each contours
        
        //Start to iterate to each contour founded
        vector<vector<Point> >::iterator itc = contours.begin();
        vector<Rect> vecRect;
        
        //Remove patch that are no inside limits of aspect ratio and area.
        //将不符合特定尺寸的图块排除出去
        while (itc != contours.end())
        {
            cv::Rect mr = boundingRect(Mat(*itc));
            Mat auxRoi(img_threshold, mr);
            if (verifyCharSizes(auxRoi))
                vecRect.push_back(mr);
            
            ++itc;
        }
        
        if (vecRect.size() == 0)
            return -3;
        
        vector<Rect> sortedRect;
        ////对符合尺寸的图块按照从左到右进行排序
        SortRect(vecRect, sortedRect);
        
        /*vector<Rect> sortedRect(vecRect);
         std::sort
         (sortedRect.begin(), sortedRect.end(), [](const Rect &r1, const Rect &r2)
         {
         return r1.x < r2.x;
         }
         );*/
        
        int specIndex = 0;
        //获得指示城市的特定Rect,如苏A的"A"
        specIndex = GetSpecificRect(sortedRect);
        
        if (m_debug)
        {
            if (specIndex < sortedRect.size())
            {
                Mat specMat(img_threshold, sortedRect[specIndex]);
                stringstream ss(stringstream::in | stringstream::out);
                ss << "image/tmp/debug_specMat" << ".jpg";
                imwrite(ss.str(), specMat);
            }
        }
        
        //根据特定Rect向左反推出中文字符
        //这样做的主要原因是根据findContours方法很难捕捉到中文字符的准确Rect，因此仅能
        //退过特定算法来指定
        Rect chineseRect;
        if (specIndex < sortedRect.size())
            chineseRect = GetChineseRect(sortedRect[specIndex]);
        else
            return -3;
        
        if (m_debug)
        {
            Mat chineseMat(img_threshold, chineseRect);
            stringstream ss(stringstream::in | stringstream::out);
            ss << "image/tmp/debug_chineseMat" << ".jpg";
            imwrite(ss.str(), chineseMat);
        }
        
        
        //新建一个全新的排序Rect
        //将中文字符Rect第一个加进来，因为它肯定是最左边的
        //其余的Rect只按照顺序去6个，车牌只可能是7个字符！这样可以避免阴影导致的“1”字符
        vector<Rect> newSortedRect;
        newSortedRect.push_back(chineseRect);
        RebuildRect(sortedRect, newSortedRect, specIndex);
        
        if (newSortedRect.size() == 0)
            return -3;
        
        
        for (int i = 0; i < newSortedRect.size(); i++)
        {
            Rect mr = newSortedRect[i];
            Mat auxRoi(img_threshold, mr);
            
            if (1)
            {
                auxRoi = preprocessChar(auxRoi);
                if (m_debug)
                {
                    stringstream ss(stringstream::in | stringstream::out);
                    ss << "image/tmp/debug_char_auxRoi_" << (i+staticIndex) << ".jpg";
                    imwrite(ss.str(), auxRoi);
                }
                resultVec.push_back(auxRoi);
            }
        }
        staticIndex+=newSortedRect.size();
        
        return 0;
    }
    
    //! 将Rect按位置从左到右进行排序
    int CCharsSegment::SortRect(const vector<Rect>& vecRect, vector<Rect>& out)
    {
        vector<int> orderIndex;
        vector<int> xpositions;
        
        for (int i = 0; i < vecRect.size(); i++)
        {
            orderIndex.push_back(i);
            xpositions.push_back(vecRect[i].x);
        }
        
        float min = xpositions[0];
        int minIdx = 0;
        for (int i = 0; i< xpositions.size(); i++)
        {
            min = xpositions[i];
            minIdx = i;
            for (int j = i; j<xpositions.size(); j++)
            {
                if (xpositions[j]<min){
                    min = xpositions[j];
                    minIdx = j;
                }
            }
            int aux_i = orderIndex[i];
            int aux_min = orderIndex[minIdx];
            orderIndex[i] = aux_min;
            orderIndex[minIdx] = aux_i;
            
            float aux_xi = xpositions[i];
            float aux_xmin = xpositions[minIdx];
            xpositions[i] = aux_xmin;
            xpositions[minIdx] = aux_xi;
        }
        
        for (int i = 0; i<orderIndex.size(); i++)
        {
            out.push_back(vecRect[orderIndex[i]]);
        }
        
        return 0;
    }
    
    //! 根据特殊车牌来构造猜测中文字符的位置和大小
    Rect CCharsSegment::GetChineseRect(const Rect rectSpe)
    {
        int height = rectSpe.height;
        float newwidth = rectSpe.width * 1.15;
        int x = rectSpe.x;
        int y = rectSpe.y;
        
        int newx = x - int(newwidth * 1.15);
        newx = newx > 0 ? newx : 0;
        
        Rect a(newx, y, int(newwidth), height);
        
        return a;
    }
    
    //! 找出指示城市的字符的Rect，例如苏A7003X，就是"A"的位置
    int CCharsSegment::GetSpecificRect(const vector<Rect>& vecRect)
    {
        vector<int> xpositions;
        int maxHeight = 0;
        int maxWidth = 0;
        
        for (int i = 0; i < vecRect.size(); i++)
        {
            xpositions.push_back(vecRect[i].x);
            
            if (vecRect[i].height > maxHeight)
            {
                maxHeight = vecRect[i].height;
            }
            if (vecRect[i].width > maxWidth)
            {
                maxWidth = vecRect[i].width;
            }
        }
        
        int specIndex = 0;
        for (int i = 0; i < vecRect.size(); i++)
        {
            Rect mr = vecRect[i];
            int midx = mr.x + mr.width / 2;
            
            //如果一个字符有一定的大小，并且在整个车牌的1/7到2/7之间，则是我们要找的特殊字符
            //当前字符和下个字符的距离在一定的范围内
            if ((mr.width > maxWidth * 0.8 || mr.height > maxHeight * 0.8) &&
                (midx < int(m_theMatWidth / 7) * 2 && midx > int(m_theMatWidth / 7) * 1))
            {
                specIndex = i;
            }
        }
        
        return specIndex;
    }
    
    //! 这个函数做两个事情
    //  1.把特殊字符Rect左边的全部Rect去掉，后面再重建中文字符的位置。
    //  2.从特殊字符Rect开始，依次选择6个Rect，多余的舍去。
    int CCharsSegment::RebuildRect(const vector<Rect>& vecRect, vector<Rect>& outRect, int specIndex)
    {
        //最大只能有7个Rect,减去中文的就只有6个Rect
        //int count = 6;
        
        //for (int i = 0; i < vecRect.size(); i++)
        //{
        //	//将特殊字符左边的Rect去掉，这个可能会去掉中文Rect，不过没关系，我们后面会重建。
        //	if (i < specIndex)
        //		continue;
        
        //	outRect.push_back(vecRect[i]);
        //	if (!--count)
        //		break;
        //}
        
        int count = 6;
        for (size_t i = specIndex; i < vecRect.size() && count; ++i, --count) {
            outRect.push_back(vecRect[i]);
        }
        
        return 0;
    }
    
}	/*! \namespace easypr*/
