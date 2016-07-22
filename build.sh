#!/usr/bin/env bash

cd `dirname $0`
__dirname=`pwd`
PLATFORM=`uname | awk '{print tolower($1)}'`
BUILD_TMP_PATH=${__dirname}/_build

BINARY_NAME=demo_${PLATFORM}_amd64
BINARY_DIR=${__dirname}/bin
BINARY_FULL_PATH=${BINARY_DIR}/${BINARY_NAME}

BUILD_BINARY_FULL_PATH=${BUILD_TMP_PATH}/${BINARY_NAME}

# cleanup
if [ -f ${BUILD_BINARY_FULL_PATH} ];then
  rm ${BUILD_BINARY_FULL_PATH}
fi

# create tmp dir for cmake
mkdir -p ${BUILD_TMP_PATH}

# Generate Makefile
cd ${BUILD_TMP_PATH}
  cmake --clean-first -DCMAKE_BUILD_TYPE=Release ${__dirname}
cd ${__dirname}

# Compile
if [ $? -eq 0 ];then
  make --jobs 4 --directory ${BUILD_TMP_PATH}
fi

if [ -f ${BUILD_BINARY_FULL_PATH} ];then
  cp ${BUILD_BINARY_FULL_PATH} ${BINARY_DIR}
fi

cp ${BUILD_BINARY_FULL_PATH} demo
