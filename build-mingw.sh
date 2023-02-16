#!/bin/bash

if [ ! -d cmake-build-mingw ] 
then
	mkdir cmake-build-mingw
fi

cd cmake-build-mingw
cmake -D MINGW=1 -D CMAKE_TOOLCHAIN_FILE=../cmake/mingw-w64-x86_64.cmake ..
make
