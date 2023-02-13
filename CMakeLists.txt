################################################################################
##
## MIT License
##
## Copyright (c) 2023 erysdren (it/she)
##
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
##
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.
## 
################################################################################

## CMake
cmake_minimum_required(VERSION 3.13)
project(QCVM LANGUAGES C)
set(CMAKE_BUILD_TYPE Release)

## Options
option(QCVM_BUILD_QC "Build Example QuakeC" ON)
option(QCVM_BUILD_TESTAPP "Build Test Application" ON)

## QCVM Sources
set(QCVM_SOURCES
	qcvm/qcvm_bootstrap.c
	qcvm/qcvm_qclib.c
	qcvm/qcvm_runtime.c
	qcvm/qcvm_utils.c
)

## QCVM Library
add_library(qcvm_static ${QCVM_SOURCES})
target_include_directories(qcvm_static PUBLIC qcvm/)

add_library(qcvm_shared SHARED ${QCVM_SOURCES})
target_include_directories(qcvm_shared PUBLIC qcvm/)

## Compile QC test environment
if(QCVM_BUILD_QC)

	find_program(fteqcc64 fteqcc64 REQUIRED)

	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/testqc.dat
		COMMAND fteqcc64 ${CMAKE_SOURCE_DIR}/qc/testqc.src
	)

	add_custom_target(testqc ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/testqc.dat)

endif()

## QCVM Test Application
if(QCVM_BUILD_TESTAPP)

	## Sources
	set(QCVM_TEST_SOURCES
		test/test.c
	)

	## Executable
	add_executable(test ${QCVM_TEST_SOURCES})
	target_link_libraries(test PUBLIC qcvm_static)

endif()
