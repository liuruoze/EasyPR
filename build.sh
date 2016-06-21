#!/usr/bin/env bash

__dirname=`dirname $0`
BUILD_TMP_PATH=${__dirname}/_build

mkdir -p ${BUILD_TMP_PATH} && cd ${BUILD_TMP_PATH}

# Generate Makefile
cmake --clean-first -DCMAKE_BUILD_TYPE=Release ..

# Compile
if [ -f 'Makefile' ];then
  make --jobs 4
fi

# Copy binary files to bin/
BINARY_PATH=../bin

cp libeasypr.a ${BINARY_PATH}
cp demo_linux_amd64 ${BINARY_PATH}

# Run demo program
cd .. && bin/demo_linux_amd64
