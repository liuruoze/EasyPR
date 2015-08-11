#!/usr/bin/env bash

BUILD_PATH="_build/"

if [ ! -e $BUILD_PATH ]; then
    mkdir $BUILD_PATH
fi

cd _build/
cmake ../
make -j 4
