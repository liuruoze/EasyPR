#!/usr/bin/env bash

build_tmp="_build/"

mkdir ${build_tmp} 2>/dev/null
cd ${build_tmp}

cmake --clean-first ..
make -j 4 --makefile Makefile
