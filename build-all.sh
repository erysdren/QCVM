#!/bin/bash

source ./build.sh
cd ..

source ./build-djgpp.sh
cd ..

source ./build-watcom.sh
cd ..

source ./build-mingw.sh
cd ..
