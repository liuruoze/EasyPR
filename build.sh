#!/usr/bin/env bash

__dirname=`dirname $0`
BUILD_TMP_PATH=${__dirname}/_build

# cleanup
rm -r bin/* 2>/dev/null

# create tmp dir for cmake
mkdir -p ${BUILD_TMP_PATH}

cd ${BUILD_TMP_PATH}

# Generate Makefile
cmake --clean-first -DCMAKE_BUILD_TYPE=Release ..

# Compile
if [ -f 'Makefile' ];then
  make --jobs 4
fi

if [ $? -eq 0 ];then
  cp demo_*_amd64 ../bin
fi
