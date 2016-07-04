/* Redistribution and use in source and binary forms, with or
* without modification, are permitted provided that the following
* conditions are met:
* 	Redistributions of source code must retain the above
* 	copyright notice, this list of conditions and the following
* 	disclaimer.
* 	Redistributions in binary form must reproduce the above
* 	copyright notice, this list of conditions and the following
* 	disclaimer in the documentation and/or other materials
* 	provided with the distribution.
* 	The name of Contributor may not be used to endorse or
* 	promote products derived from this software without
* 	specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
* Copyright? 2009, Liu Liu All rights reserved.
*
* OpenCV functions for MSER extraction
*
* 1. there are two different implementation of MSER, one for gray image, one for color image
* 2. the gray image algorithm is taken from: Linear Time Maximally Stable Extremal Regions;
*    the paper claims to be faster than union-find method;
*    it actually get 1.5~2m/s on my centrino L7200 1.2GHz laptop.
* 3. the color image algorithm is taken from: Maximally Stable Colour Regions for Recognition and Match;
*    it should be much slower than gray image method ( 3~4 times );
*    the chi_table.h file is taken directly from paper's source code which is distributed under GPL.
* 4. though the name is *contours*, the result actually is a list of point set.

Modifer:liuruoze


*/


#ifndef __OPENCV_PRECOMP_H__
#define __OPENCV_PRECOMP_H__

#include "opencv2/features2d.hpp"
#include "opencv2/imgproc.hpp"

#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"

#include <algorithm>

#ifdef HAVE_TEGRA_OPTIMIZATION
#include "opencv2/features2d/features2d_tegra.hpp"
#endif

#endif

#include "opencv2/imgproc/imgproc_c.h"
#include <limits>
#include "mser2.hpp"

namespace cv
{

  using std::vector;
  
  Rect mergeRect(const Rect& a, const Rect& b) {
    Rect c;
    int x1 = a.x < b.x ? a.x : b.x;
    int y1 = a.y < b.y ? a.y : b.y;
    c.width = (a.x + a.width > b.x + b.width ? a.x + a.width : b.x + b.width) - x1;
    c.height = (a.y + a.height > b.y + b.height ? a.y + a.height : b.y + b.height) - y1;
    c.x = x1;
    c.y = y1;
    return c;
  }


  class MSER_Impl2 : public MSER2
  {
  public:

    struct Params
    {
      Params(bool _useOpt = false, bool _subPath = false, bool _realMSER = false, bool _usePrune = false,
      int _delta = 5, int _min_area = 60, int _max_area = 14400,
      double _max_variation = 0.25, double _min_diversity = .2,
      int _max_evolution = 200, double _area_threshold = 1.01,
      double _min_margin = 0.003, int _edge_blur_size = 5)
      {
        useOpt = _useOpt;
        subPath = _subPath;
        realMSER = _realMSER;
        usePrune = _usePrune;

        delta = _delta;
        minArea = _min_area;
        maxArea = _max_area;
        maxVariation = _max_variation;
        minDiversity = _min_diversity;
        maxEvolution = _max_evolution;
        areaThreshold = _area_threshold;
        minMargin = _min_margin;
        edgeBlurSize = _edge_blur_size;
        pass2Only = false;
      }


      bool useOpt;
      bool subPath;
      bool realMSER;
      bool usePrune;

      int delta;
      int minArea;
      int maxArea;
      double maxVariation;
      double minDiversity;
      bool pass2Only;

      int maxEvolution;
      double areaThreshold;
      double minMargin;
      int edgeBlurSize;
    };

    explicit MSER_Impl2(const Params& _params) : params(_params) {}

    virtual ~MSER_Impl2() {}

    void setDelta(int delta) { params.delta = delta; }
    int getDelta() const { return params.delta; }

    void setMinArea(int minArea) { params.minArea = minArea; }
    int getMinArea() const { return params.minArea; }

    void setMaxArea(int maxArea) { params.maxArea = maxArea; }
    int getMaxArea() const { return params.maxArea; }

    void setPass2Only(bool f) { params.pass2Only = f; }
    bool getPass2Only() const { return params.pass2Only; }

    enum { DIR_SHIFT = 29, NEXT_MASK = ((1 << DIR_SHIFT) - 1) };

    struct Pixel
    {
      Pixel() : val(0) {}
      Pixel(int _val) : val(_val) {}

      int getGray(const Pixel* ptr0, const uchar* imgptr0, int mask) const
      {
        return imgptr0[this - ptr0] ^ mask;
      }
      int getNext() const { return (val & NEXT_MASK); }
      void setNext(int next) { val = (val & ~NEXT_MASK) | next; }

      int getDir() const { return (int)((unsigned)val >> DIR_SHIFT); }
      void setDir(int dir) { val = (val & NEXT_MASK) | (dir << DIR_SHIFT); }
      bool isVisited() const { return (val & ~NEXT_MASK) != 0; }

      int val;
    };
    typedef int PPixel;

    struct WParams
    {
      Params p;
      vector<vector<Point> >* msers;
      vector<Rect>* bboxvec;
      Pixel* pix0;
      int step;
      float similyThresh;
    };

    // the history of region grown
    struct CompHistory
    {
      CompHistory()
      {
        parent_ = child_ = next_ = 0;
        val = size = 0;
        var = -1.f;
        head = 0;
        checked = false;
        rect = Rect(0, 0, 0, 0);
      }

      void updateTree(WParams& wp, CompHistory** _h0, CompHistory** _h1, bool final)
      {
        if (var >= 0.f)
          return;
        int delta = wp.p.delta;

        CompHistory* h0_ = 0, *h1_ = 0;
        CompHistory* c = child_;
        if (size >= wp.p.minArea)
        {
          for (; c != 0; c = c->next_)
          {
            if (c->var < 0.f)
              c->updateTree(wp, c == child_ ? &h0_ : 0, c == child_ ? &h1_ : 0, final);

            if (c->var < 0.f)
              return;
          }
        }

        // find h0 and h1 such that:
        //    h0->val >= h->val - delta and (h0->parent == 0 or h0->parent->val < h->val - delta)
        //    h1->val <= h->val + delta and (h1->child == 0 or h1->child->val < h->val + delta)
        // then we will adjust h0 and h1 as h moves towards latest
        CompHistory* h0 = this, *h1 = h1_ && h1_->size > size ? h1_ : this;
        if (h0_)
        {
          for (h0 = h0_; h0 != this && h0->val < val - delta; h0 = h0->parent_)
            ;
        }
        else
        {
          for (; h0->child_ && h0->child_->val >= val - delta; h0 = h0->child_)
            ;
        }

        for (; h1->parent_ && h1->parent_->val <= val + delta; h1 = h1->parent_)
          ;

        if (_h0) *_h0 = h0;
        if (_h1) *_h1 = h1;

        // when we do not well-defined ER(h->val + delta), we stop
        // the process of computing variances unless we are at the final step
        if (!final && !h1->parent_ && h1->val < val + delta)
          return;

        var = (float)(h1->size - h0->size) / size;

        //printf("var: %f \n", var);

        // var regulartion 
        // come form 2014 PAMI "Robust Text Detection in Natural Scene Images"
        if (wp.p.useOpt){

          float width = (float)rect.width;
          float height = (float)rect.height;

          float aspect = width / height;
          float theta_1 = 0.01f;
          float theta_2 = 0.35f;
          float a_max = 1.2f;
          float a_min = 0.3f;

          // CV_Assert(aspect < a_max);
          // CV_Assert(aspect > a_min);

          if (aspect > a_max) {
            var = var + theta_1 * (aspect - a_max);
          }
          else if (aspect < a_min) {
            var = var + theta_2 * (a_min - aspect);
          }
          else {
            var = var;
          }

          // end var regulartion 
        }

        c = child_;
        for (; c != 0; c = c->next_)
          c->checkAndCapture(wp);
        if (final && !parent_)
          checkAndCapture(wp);
      }

      // changed bu liuruoze
      void checkAndCapture(WParams& wp)
      {
        if (checked)
          return;
        checked = true;

        if (size < wp.p.minArea || size > wp.p.maxArea || var < 0.f || var > wp.p.maxVariation)
          return;

        int sub_path_length = 0;

        if (child_)
        {
          CompHistory* c = child_;
          for (; c != 0; c = c->next_)
          {
            sub_path_length = 0;

            if (wp.p.subPath)
            {
              Rect rect_inter = rect & c->rect;
              Rect rect_union = rect | c->rect;

              float area_inter = float(rect_inter.area());
              float area_union = float(rect_union.area());

              float similty = area_inter / area_union;

              if (similty < wp.similyThresh)
                continue;
            }

            if (c->var >= 0.f && var > c->var)
              return;

            sub_path_length++;

            if (wp.p.realMSER)
            {
              CompHistory* d = c->child_;
              CompHistory* parent = c;

              for (; d != 0; d = d->child_)
              {
                if (wp.p.subPath)
                {
                  Rect rect_inter = parent->rect & d->rect;
                  Rect rect_union = parent->rect | d->rect;

                  float area_inter = float(rect_inter.area());
                  float area_union = float(rect_union.area());

                  float similty = area_inter / area_union;

                  if (similty < wp.similyThresh)
                    break;
                }

                if (d->var >= 0.f && var > d->var)
                  return;

                parent = d;
                sub_path_length++;
              }
            }
          }
        }

        if (parent_)
        {
          CompHistory* d = parent_;
          CompHistory* child = this;

          if (wp.p.realMSER)
          {
            for (; d != 0; d = d->parent_)
            {
              if (wp.p.subPath)
              {
                Rect rect_inter = child->rect & d->rect;
                Rect rect_union = child->rect | d->rect;

                float area_inter = float(rect_inter.area());
                float area_union = float(rect_union.area());

                float similty = area_inter / area_union;

                if (similty < wp.similyThresh)
                  break;
              }

              if (d->var >= 0.f && var > d->var)
                return;

              child = d;
              sub_path_length++;
            }
          }
          else
          {
            bool compareToParent = true;
            if (wp.p.subPath)
            {
              Rect rect_inter = child->rect & d->rect;
              Rect rect_union = child->rect | d->rect;

              float area_inter = float(rect_inter.area());
              float area_union = float(rect_union.area());

              float similty = area_inter / area_union;

              if (similty < wp.similyThresh)
                compareToParent = false;
            }

            if (compareToParent && d->var >= 0.f && var > d->var)
              return;

            sub_path_length++;
          }

        }

        if (wp.p.usePrune && sub_path_length < 3)
          return;

         //more efficent to get the rect, dismiss the regions;
         //changed by liuruoze
         //wp.bboxvec->push_back(rect);

        if (child_)
        {
          CompHistory* c = child_;
          for (; c != 0; c = c->next_)
          {
            if (c->var >= 0.f && var > c->var)
              return;
          }
        }

        if (parent_ && parent_->var >= 0.f && var >= parent_->var)
          return;

        int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN, j = 0;
        wp.msers->push_back(vector<Point>());
        vector<Point>& region = wp.msers->back();
        region.resize(size);
        const Pixel* pix0 = wp.pix0;
        int step = wp.step;

        for (PPixel pix = head; j < size; j++, pix = pix0[pix].getNext())
        {
          int y = pix / step;
          int x = pix - y*step;

          // modifier for more fast
          //xmin = std::min(xmin, x);
          //xmax = std::max(xmax, x);
          //ymin = std::min(ymin, y);
          //ymax = std::max(ymax, y);
          xmin = xmin < x ? xmin : x;
          xmax = xmax > x ? xmax : x;
          ymin = ymin < y ? ymin : y;
          ymax = ymax > y ? ymax : y;

          region[j] = Point(x, y);
        }
        wp.bboxvec->push_back(Rect(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1));
      }

      CompHistory* child_;
      CompHistory* parent_;
      CompHistory* next_;
      int val;
      int size;
      float var;
      PPixel head;
      bool checked;
      Rect rect;
    };

    struct ConnectedComp
    {
      ConnectedComp()
      {
        init(0);
      }

      void init(int gray)
      {
        head = tail = 0;
        history = 0;
        size = 0;
        gray_level = gray;
        rect = Rect(0, 0, 0, 0);
      }

      // add history chunk to a connected component
      void growHistory(CompHistory*& hptr, WParams& wp, int new_gray_level, bool final, bool force = false)
      {
        bool update = final;
        if (new_gray_level < 0)
          new_gray_level = gray_level;
        if (!history || (history->size != size && size > 0 &&
          (gray_level != history->val || force)))
        {
          CompHistory* h = hptr++;
          h->parent_ = 0;
          h->child_ = history;
          h->next_ = 0;
          if (history)
            history->parent_ = h;
          h->val = gray_level;
          h->size = size;
          h->head = head;
          h->rect = rect;

          history = h;
          h->var = FLT_MAX;
          h->checked = true;
          if (h->size >= wp.p.minArea)
          {
            h->var = -1.f;
            h->checked = false;
            update = true;
          }
        }
        gray_level = new_gray_level;
        if (update && history)
          history->updateTree(wp, 0, 0, final);
      }

      // merging two connected components
      void merge(ConnectedComp* comp1, ConnectedComp* comp2,
        CompHistory*& hptr, WParams& wp)
      {
        comp1->growHistory(hptr, wp, -1, false);
        comp2->growHistory(hptr, wp, -1, false);

        if (comp1->size < comp2->size)
          std::swap(comp1, comp2);

        if (comp2->size == 0)
        {
          gray_level = comp1->gray_level;
          head = comp1->head;
          tail = comp1->tail;
          size = comp1->size;
          history = comp1->history;

          // add by liuruoze
          if (wp.p.useOpt)
            rect = comp1->rect;

          return;
        }

        CompHistory* h1 = comp1->history;
        CompHistory* h2 = comp2->history;

        gray_level = std::max(comp1->gray_level, comp2->gray_level);
        history = comp1->history;
        wp.pix0[comp1->tail].setNext(comp2->head);

        head = comp1->head;
        tail = comp2->tail;
        size = comp1->size + comp2->size;

        // add by liuruoze
        if (wp.p.useOpt) {
          //rect = comp1->rect | comp2->rect;
          rect = mergeRect(comp1->rect, comp2->rect);
        }
          

        bool keep_2nd = h2->size > wp.p.minArea;
        growHistory(hptr, wp, -1, false, keep_2nd);
        if (keep_2nd)
        {
          h1->next_ = h2;
          h2->parent_ = history;
        }
      }

      PPixel head;
      PPixel tail;
      CompHistory* history;
      int gray_level;
      int size;
      Rect rect;
    };

    void detectRegions(InputArray _src, vector<vector<Point>>& msers_blue, vector<Rect>& bboxes_blue,
      vector<vector<Point>>& msers_yellow, vector<Rect>& bboxes_yellow);

    void preprocess1(const Mat& img, int* level_size)
    {
      memset(level_size, 0, 256 * sizeof(level_size[0]));

      int i, j, cols = img.cols, rows = img.rows;
      int step = cols;
      pixbuf.resize(step*rows);
      heapbuf.resize(cols*rows + 256);
      histbuf.resize(cols*rows);
      Pixel borderpix;
      borderpix.setDir(5);

      for (j = 0; j < step; j++)
      {
        pixbuf[j] = pixbuf[j + (rows - 1)*step] = borderpix;
      }

      for (i = 1; i < rows - 1; i++)
      {
        const uchar* imgptr = img.ptr(i);
        Pixel* pptr = &pixbuf[i*step];
        pptr[0] = pptr[cols - 1] = borderpix;
        for (j = 1; j < cols - 1; j++)
        {
          int val = imgptr[j];
          level_size[val]++;
          pptr[j].val = 0;
        }
      }
    }

    void preprocess2(const Mat& img, int* level_size)
    {
      int i;

      for (i = 0; i < 128; i++)
        std::swap(level_size[i], level_size[255 - i]);

      if (!params.pass2Only)
      {
        int j, cols = img.cols, rows = img.rows;
        int step = cols;
        for (i = 1; i < rows - 1; i++)
        {
          Pixel* pptr = &pixbuf[i*step + 1];
          for (j = 1; j < cols - 1; j++)
          {
            pptr[j].val = 0;
          }
        }
      }
    }

    void pass(const Mat& img, vector<vector<Point> >& msers, vector<Rect>& bboxvec,
      Size size, const int* level_size, int mask)
    {
      CompHistory* histptr = &histbuf[0];
      int step = size.width;
      Pixel *ptr0 = &pixbuf[0], *ptr = &ptr0[step + 1];
      const uchar* imgptr0 = img.ptr();
      Pixel** heap[256];
      ConnectedComp comp[257];
      ConnectedComp* comptr = &comp[0];
      WParams wp;
      wp.p = params;
      wp.msers = &msers;
      wp.bboxvec = &bboxvec;
      wp.pix0 = ptr0;
      wp.step = step;
      wp.similyThresh = 0.7f;

      heap[0] = &heapbuf[0];
      heap[0][0] = 0;

      for (int i = 1; i < 256; i++)
      {
        heap[i] = heap[i - 1] + level_size[i - 1] + 1;
        heap[i][0] = 0;
      }

      comptr->gray_level = 256;
      comptr++;
      comptr->gray_level = ptr->getGray(ptr0, imgptr0, mask);
      ptr->setDir(1);
      int dir[] = { 0, 1, step, -1, -step };
      for (;;)
      {
        int curr_gray = ptr->getGray(ptr0, imgptr0, mask);
        int nbr_idx = ptr->getDir();
        // take tour of all the 4 directions
        for (; nbr_idx <= 4; nbr_idx++)
        {
          // get the neighbor
          Pixel* ptr_nbr = ptr + dir[nbr_idx];
          if (!ptr_nbr->isVisited())
          {
            // set dir=1, next=0
            ptr_nbr->val = 1 << DIR_SHIFT;
            int nbr_gray = ptr_nbr->getGray(ptr0, imgptr0, mask);
            if (nbr_gray < curr_gray)
            {
              // when the value of neighbor smaller than current
              // push current to boundary heap and make the neighbor to be the current one
              // create an empty comp
              *(++heap[curr_gray]) = ptr;
              ptr->val = (nbr_idx + 1) << DIR_SHIFT;
              ptr = ptr_nbr;
              comptr++;
              comptr->init(nbr_gray);
              curr_gray = nbr_gray;
              nbr_idx = 0;
              continue;
            }
            // otherwise, push the neighbor to boundary heap
            *(++heap[nbr_gray]) = ptr_nbr;
          }
        }

        // set dir = nbr_idx, next = 0
        ptr->val = nbr_idx << DIR_SHIFT;
        int ptrofs = (int)(ptr - ptr0);
        CV_Assert(ptrofs != 0);

        int y = ptrofs / step;
        int x = ptrofs - y * step;

        // add a pixel to the pixel list
        // changed by liuruoze
        if (comptr->tail)
        {
          ptr0[comptr->tail].setNext(ptrofs);
          if (params.useOpt) {
            //comptr->rect |= Rect(x, y, 1, 1);
            comptr->rect = mergeRect(comptr->rect, Rect(x, y, 1, 1));
          }
            
        }
        else
        {
          // printf("x : %i \n", x);
          // printf("y : %i \n", y);

          comptr->head = ptrofs;
          if (params.useOpt)
            comptr->rect = Rect(x, y, 1, 1);
        }

        comptr->tail = ptrofs;
        comptr->size++;
        // get the next pixel from boundary heap
        if (*heap[curr_gray])
        {
          ptr = *heap[curr_gray];
          heap[curr_gray]--;
        }
        else
        {
          for (curr_gray++; curr_gray < 256; curr_gray++)
          {
            if (*heap[curr_gray])
              break;
          }
          if (curr_gray >= 256)
            break;

          ptr = *heap[curr_gray];
          heap[curr_gray]--;

          if (curr_gray < comptr[-1].gray_level)
            comptr->growHistory(histptr, wp, curr_gray, false);
          else
          {
            // keep merging top two comp in stack until the gray level >= pixel_val
            for (;;)
            {
              comptr--;
              comptr->merge(comptr, comptr + 1, histptr, wp);
              if (curr_gray <= comptr[0].gray_level)
                break;
              if (curr_gray < comptr[-1].gray_level)
              {
                comptr->growHistory(histptr, wp, curr_gray, false);
                break;
              }
            }
          }
        }
      }

      for (; comptr->gray_level != 256; comptr--)
      {
        comptr->growHistory(histptr, wp, 256, true, true);
      }
    }

    Mat tempsrc;
    vector<Pixel> pixbuf;
    vector<Pixel*> heapbuf;
    vector<CompHistory> histbuf;
    
    Params params;

  };

  void MSER_Impl2::detectRegions(InputArray _src, vector<vector<Point>>& msers_blue, vector<Rect>& bboxes_blue,
  vector<vector<Point>>& msers_yellow, vector<Rect>& bboxes_yellow)
  {
    Mat src = _src.getMat();
    size_t npix = src.total();

    if (npix == 0)
      return;

    Size size = src.size();

    if (src.type() == CV_8U)
    {
      int level_size[256];
      if (!src.isContinuous())
      {
        src.copyTo(tempsrc);
        src = tempsrc;
      }

      // darker to brighter (MSER+)
      // dont need when plate is blue
      preprocess1(src, level_size);
      if (!params.pass2Only)
        pass(src, msers_yellow, bboxes_yellow, size, level_size, 0);

      // brighter to darker (MSER-)
      preprocess2(src, level_size);
      pass(src, msers_blue, bboxes_blue, size, level_size, 255);
    }
  }


  Ptr<MSER2> MSER2::create(int _delta, int _min_area, int _max_area,
    double _max_variation, double _min_diversity,
    int _max_evolution, double _area_threshold,
    double _min_margin, int _edge_blur_size)
  {

    //printf("better mser 2 \n");

    bool useOpt = true;
    bool subPath = true;
    bool realMSER = true;
    bool usePrune = true;

    return makePtr<MSER_Impl2>(
      MSER_Impl2::Params(useOpt, subPath, realMSER, usePrune, _delta, _min_area, _max_area,
      _max_variation, _min_diversity,
      _max_evolution, _area_threshold,
      _min_margin, _edge_blur_size));
  }

  
}
