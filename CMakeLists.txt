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
project(QCVM DESCRIPTION "QuakeC Virtual Machine" HOMEPAGE_URL https://github.com/erysdren/QCVM LANGUAGES C VERSION 0.0.1)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

## Options
option(QCVM_BUILD_EXAMPLES "Build Example Applications" ON)
option(QCVM_BUILD_QCPONG "Build Example Game QCPONG" ON)
option(QCVM_BUILD_QCPKG "Build QCPKG tool" ON)
option(QCVM_INSTALL_TARGET "Enable install target" ON)

## QCVM Sources
set(QCVM_SOURCES
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_bootstrap.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_entities.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_exports.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_fields.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_functions.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_globals.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_opcodes.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_parameters.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_return.c
	${PROJECT_SOURCE_DIR}/qcvm/qcvm_runtime.c
)

## QCVM Library
add_library(qcvm ${QCVM_SOURCES})
target_include_directories(qcvm PUBLIC ${PROJECT_SOURCE_DIR}/qcvm)

## QCLIB Sources
set(QCLIB_SOURCES
	${PROJECT_SOURCE_DIR}/qclib/qclib.c
)

## QCLIB library
add_library(qclib ${QCLIB_SOURCES})
target_link_libraries(qclib PUBLIC qcvm)
target_include_directories(qclib PUBLIC ${PROJECT_SOURCE_DIR}/qclib)

# headers
set(QCVM_HEADERS
	${PROJECT_SOURCE_DIR}/qcvm/qcvm.h
	${PROJECT_SOURCE_DIR}/qclib/qclib.h
)

if(QCVM_INSTALL_TARGET)
	include(${PROJECT_SOURCE_DIR}/cmake/install.cmake)
endif()

# QCPKG tool
if(QCVM_BUILD_QCPKG)

	# QCPKG sources
	set(QCPKG_SOURCES
		${PROJECT_SOURCE_DIR}/qcpkg/main.c
	)

	## QCPKG tool
	add_executable(qcpkg ${QCPKG_SOURCES})
	target_link_libraries(qcpkg PUBLIC qcvm qclib)

endif()

# things we need to build the test applications
if(QCVM_BUILD_EXAMPLES OR QCVM_BUILD_QCPONG)
	find_program(fteqcc fteqcc)
	if(NOT fteqcc)
		message(WARNING "Can't find QuakeC compiler (fteqcc), cannot build example applications")
	endif()
endif()

if(QCVM_BUILD_QCPONG)
	find_package(SDL2)
	if(NOT SDL2_FOUND)
		message(WARNING "Can't find SDL2, cannot build example game")
	endif()
endif()

## QCVM Example Applications
if(QCVM_BUILD_EXAMPLES AND fteqcc)

	#
	# testapp 1
	#

	## QC
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/test.dat
		COMMAND fteqcc ${PROJECT_SOURCE_DIR}/examples/qcvm_test/test.src -o ${PROJECT_BINARY_DIR}/test.dat
	)
	add_custom_target(test_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/test.dat)

	## Sources
	set(QCVM_TEST_SOURCES
		${PROJECT_SOURCE_DIR}/examples/qcvm_test/main.c
	)

	## Executable
	add_executable(test ${QCVM_TEST_SOURCES})
	target_link_libraries(test PUBLIC qcvm qclib)

	#
	# testapp 2
	#

	## QC
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/test2.dat
		COMMAND fteqcc ${PROJECT_SOURCE_DIR}/examples/qcvm_test_fte/test2.src -o ${PROJECT_BINARY_DIR}/test2.dat
	)
	add_custom_target(test2_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/test2.dat)

	## Sources
	set(QCVM_TEST_FTE_SOURCES
		${PROJECT_SOURCE_DIR}/examples/qcvm_test_fte/main.c
	)

	## Executable
	add_executable(test2 ${QCVM_TEST_FTE_SOURCES})
	target_link_libraries(test2 PUBLIC qcvm qclib)

	#
	# testapp 3
	#

	## QC
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/module1.dat
		COMMAND fteqcc ${PROJECT_SOURCE_DIR}/examples/module_test/module1.src -o ${PROJECT_BINARY_DIR}/module1.dat
	)
	add_custom_command(
		OUTPUT ${PROJECT_BINARY_DIR}/module2.dat
		COMMAND fteqcc ${PROJECT_SOURCE_DIR}/examples/module_test/module2.src -o ${PROJECT_BINARY_DIR}/module2.dat
	)
	add_custom_target(module1_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/module1.dat)
	add_custom_target(module2_dat ALL DEPENDS ${PROJECT_BINARY_DIR}/module2.dat)

	## Sources
	set(QCVM_MODULE_TEST_SOURCES
		${PROJECT_SOURCE_DIR}/examples/module_test/module_test.c
	)

	## Executable
	add_executable(module_test ${QCVM_MODULE_TEST_SOURCES})
	target_link_libraries(module_test PUBLIC qcvm qclib)

endif()

## QCVM Example Game
if(QCVM_BUILD_QCPONG AND SDL2_FOUND AND fteqcc)

	## QC
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/qcpong.dat
		COMMAND fteqcc ${PROJECT_SOURCE_DIR}/examples/qcpong/qcpong.src -o ${PROJECT_BINARY_DIR}/qcpong.dat
	)
	add_custom_target(qcpong_dat ALL DEPENDS ${CMAKE_BINARY_DIR}/qcpong.dat)

	## Sources
	set(QCPONG_SOURCES
		${PROJECT_SOURCE_DIR}/examples/qcpong/main.c
		${PROJECT_SOURCE_DIR}/examples/qcpong/stb_image_write.c
	)

	## Executable
	add_executable(qcpong ${QCPONG_SOURCES})
	target_link_libraries(qcpong PUBLIC qcvm qclib)
	target_link_libraries(qcpong PUBLIC SDL2::SDL2)

endif()
