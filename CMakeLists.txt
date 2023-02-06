###############################################################################
##
## QCVM is Copyright (C) 2023-2024 erysdren (it/she)
## Quake is Copyright (C) 1996-1997 Id Software, Inc.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
##
## See the GNU General Public License for more details.
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
##
###############################################################################

###############################################################################
##
## Project:			QCVM
##
## License:			GNU GPL v2
##
## Authors:			erysdren (it/she)
##
## File:			/CMakeLists.txt
##
## Description:		QCVM cmake build script
##
## Last Modified:	February 5th 2023
##
###############################################################################

## CMake
cmake_minimum_required(VERSION 3.13)
project(QCVM LANGUAGES C)
set(CMAKE_BUILD_TYPE Release)

## QCVM Sources
set(QCVM_SOURCES
	source/qcvm.c
	source/libqcvm.c
)

## QCVM test sources
set(QCVM_TEST_SOURCES
	source/test.c
)

## Library
add_library(qcvm ${QCVM_SOURCES})
target_include_directories(qcvm PUBLIC source/)

## Application
add_executable(test ${QCVM_TEST_SOURCES})
target_link_libraries(test PUBLIC qcvm)
