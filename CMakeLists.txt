cmake_minimum_required(VERSION 2.8)
project(easypr)

# c++11 required
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if (COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else ()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} doesn’t support C++11. Please upgrade or use a different C++ compiler.")
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/usr/local/opt/opencv3")
endif ()

# OpenVC3 required
find_package(OpenCV 3.1.0 REQUIRED)

# where to find header files
include_directories(.)
include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

# sub directories
add_subdirectory(thirdparty)

# sources to be compiled
set(SOURCE_FILES
        src/core/core_func.cpp
        src/core/chars_identify.cpp
        src/core/chars_recognise.cpp
        src/core/chars_segment.cpp
        src/core/feature.cpp
        src/core/plate_detect.cpp
        src/core/plate_judge.cpp
        src/core/plate_locate.cpp
        src/core/plate_recognize.cpp
        src/core/better_mser.cpp

        src/train/ann_train.cpp
        src/train/svm_train.cpp
        src/train/train.cpp

        src/preprocess/deface.cpp
        src/preprocess/gdts.cpp
        src/preprocess/mc_data.cpp

        src/util/util.cpp
        src/util/program_options.cpp
        src/util/kv.cpp
        )

# pack objects to static library
add_library(easypr STATIC ${SOURCE_FILES})

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(EXECUTABLE_NAME "demo_darwin_amd64")
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(EXECUTABLE_NAME "demo_linux_amd64")
endif ()

# test cases
add_executable(${EXECUTABLE_NAME} test/main.cpp)
# link opencv libs
target_link_libraries(${EXECUTABLE_NAME} easypr thirdparty ${OpenCV_LIBS})