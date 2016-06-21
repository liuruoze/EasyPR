cmake_minimum_required(VERSION 2.8)
project(thirdparty)

# sources to be compiled
set(SOURCE_FILES
        xmlParser/xmlParser.cpp
        erfilter.cpp
        helper.cpp
        lbp.cpp
        )

# pack objects to static library
add_library(thirdparty STATIC ${SOURCE_FILES})
