cmake_minimum_required(VERSION 3.1.0)
project(thirdparty)

# OpenVC3 required
find_package(OpenCV 3.1.0 REQUIRED)

# where to find header files
include_directories(${OpenCV_INCLUDE_DIRS})

# sources to be compiled
set(SOURCE_FILES
        xmlParser/xmlParser.cpp
        textDetect/erfilter.cpp
        LBP/helper.cpp
        LBP/lbp.cpp
        mser/mser2.cpp
        )

# pack objects to static library
add_library(thirdparty STATIC ${SOURCE_FILES})
