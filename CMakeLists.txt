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
set(CMAKE_C_STANDARD 90)

## Options
option(QCVM_BUILD_TESTAPP "Build Test Application" ON)
option(QCVM_BUILD_PASCAL_TESTAPP "Build Pascal Test Application" OFF)
option(QCVM_BUILD_QCPONG "Build Example Game" ON)
option(QCVM_BUILD_MODULE_TESTAPP "Build Module Test Application" ON)

## QCVM Sources
set(QCVM_SOURCES
	qcvm/qcvm_bootstrap.c
	qcvm/qcvm_entities.c
	qcvm/qcvm_exports.c
	qcvm/qcvm_fields.c
	qcvm/qcvm_functions.c
	qcvm/qcvm_globals.c
	qcvm/qcvm_opcodes.c
	qcvm/qcvm_parameters.c
	qcvm/qcvm_return.c
	qcvm/qcvm_runtime.c
)

## QCVM Library (static)
add_library(qcvm ${QCVM_SOURCES})
target_include_directories(qcvm PUBLIC qcvm/)

## QCVM Library (shared)
if(NOT WATCOM AND NOT DJGPP)
	add_library(qcvm_shared SHARED ${QCVM_SOURCES})
	target_include_directories(qcvm_shared PUBLIC qcvm/)
	set_target_properties(qcvm_shared PROPERTIES OUTPUT_NAME qcvm)
endif()

## EQCC Sources
set(EQCC_SOURCES
	eqcc/eqcc_compile.c
	eqcc/eqcc_main.c
	eqcc/eqcc_utils.c
)

## EQCC Executable
add_executable(eqcc ${EQCC_SOURCES})
target_link_libraries(eqcc PUBLIC qcvm qclib)
configure_file(eqcc/test.qcs ${PROJECT_BINARY_DIR}/test.qcs COPYONLY)
configure_file(eqcc/test.qch ${PROJECT_BINARY_DIR}/test.qch COPYONLY)

## QCLIB Sources
set(QCVM_SOURCES
	qclib/qclib.c
)

## QCLIB (static)
add_library(qclib ${QCVM_SOURCES})
target_link_libraries(qclib PUBLIC qcvm)
target_include_directories(qclib PUBLIC qclib/)

## QCVM Pascal Test Application
if(QCVM_BUILD_PASCAL_TESTAPP)

	## FPC
	find_program(fpc fpc REQUIRED)

	## Sources
	set(QCVM_PASCAL_TEST_SOURCES
		${PROJECT_SOURCE_DIR}/examples/pascal/pascal_test.pas
	)

	## Executable
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/pascal_test
		COMMAND fpc -FE${PROJECT_BINARY_DIR} -Fu${PROJECT_SOURCE_DIR}/bindings/pascal ${QCVM_PASCAL_TEST_SOURCES}
	)
	add_custom_target(pascal_test ALL DEPENDS ${PROJECT_BINARY_DIR}/pascal_test)

endif()

## QCVM Test Application
if(QCVM_BUILD_TESTAPP)

	## QC
	find_program(fteqcc64 fteqcc64 REQUIRED)
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/test.dat
		COMMAND fteqcc64 ${PROJECT_SOURCE_DIR}/examples/qcvm_test/test.src -o ${PROJECT_BINARY_DIR}/test.dat
	)
	add_custom_target(test_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/test.dat)

	## Sources
	set(QCVM_TEST_SOURCES
		examples/qcvm_test/main.c
	)

	## Executable
	add_executable(test ${QCVM_TEST_SOURCES})
	target_link_libraries(test PUBLIC qcvm qclib)

endif()

## QCVM Module Test Application
if(QCVM_BUILD_MODULE_TESTAPP)

	## QC
	find_program(fteqcc64 fteqcc64 REQUIRED)
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/module1.dat
		COMMAND fteqcc64 ${PROJECT_SOURCE_DIR}/examples/module_test/module1.src -o ${PROJECT_BINARY_DIR}/module1.dat
	)
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/module2.dat
		COMMAND fteqcc64 ${PROJECT_SOURCE_DIR}/examples/module_test/module2.src -o ${PROJECT_BINARY_DIR}/module2.dat
	)
	add_custom_target(module1_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/module1.dat)
	add_custom_target(module2_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/module2.dat)

	## Sources
	set(QCVM_MODULE_TEST_SOURCES
		examples/module_test/module_test.c
	)

	## Executable
	add_executable(module_test ${QCVM_MODULE_TEST_SOURCES})
	target_link_libraries(module_test PUBLIC qcvm qclib)

endif()

## QCVM Example Game
if(QCVM_BUILD_QCPONG AND NOT DJGPP AND NOT WATCOM)

	## QC
	find_program(fteqcc64 fteqcc64 REQUIRED)
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/qcpong.dat
		COMMAND fteqcc64 ${PROJECT_SOURCE_DIR}/examples/qcpong/qcpong.src -o ${PROJECT_BINARY_DIR}/qcpong.dat
	)
	add_custom_target(qcpong_dat ALL DEPENDS ${CMAKE_BINARY_DIR}/qcpong.dat)

	## Sources
	set(QCPONG_SOURCES
		examples/qcpong/main.c
		examples/qcpong/stb_image_write.c
	)

	## Dependencies
	find_package(SDL2 REQUIRED)

	## Executable
	add_executable(qcpong ${QCPONG_SOURCES})
	target_link_libraries(qcpong PUBLIC qcvm qclib)
	target_link_libraries(qcpong PUBLIC SDL2::SDL2)

endif()
