
#ifndef __PREP_H__
#define __PREP_H__

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv/cvaux.h"

#if defined (WIN32) || defined (_WIN32)
#include <objbase.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#if defined (WIN32) || defined (_WIN32)
#include <io.h>
#elif defined (linux) || defined (__linux__)
#include <sys/io.h>
#endif

#include <iostream>
#include <fstream>
#include <assert.h>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <math.h> 

using namespace std;
using namespace cv;

#endif
/* endif __PREP_H__ */
