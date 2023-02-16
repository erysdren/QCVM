#!/bin/bash

if [ ! -d cmake-build-djgpp ] 
then
	mkdir cmake-build-djgpp
fi

cd cmake-build-djgpp
cmake -D DJGPP=1 -D CMAKE_TOOLCHAIN_FILE=../cmake/i586-pc-msdosdjgpp.cmake ..
make
