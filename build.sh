#!/bin/bash

if [ ! -d cmake-build-0 ] 
then
	mkdir cmake-build-0
fi

cd cmake-build-0
cmake ..
make
