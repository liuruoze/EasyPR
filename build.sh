#!/usr/bin/env bash

__dirname=`dirname $0`
BUILD_TMP_PATH=${__dirname}/_build
EXECUTABLE=demo_linux_amd64

# cleanup
rm bin/${EXECUTABLE}

# create tmp dir for cmake
mkdir -p ${BUILD_TMP_PATH} && cd ${BUILD_TMP_PATH}

# Generate Makefile
cmake --clean-first -DCMAKE_BUILD_TYPE=Release ..

# Compile
if [ -f 'Makefile' ];then
  make --jobs 4
fi

if [ $? -eq 0 ];then
  # Copy binary files to bin/
  BINARY_PATH=../bin

  cp ${EXECUTABLE} ${BINARY_PATH}

  # Run demo program
  cd .. && bin/demo_linux_amd64
fi
