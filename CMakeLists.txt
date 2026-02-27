################################################################################
##
## MIT License
##
## Copyright (c) 2023-2026 erysdren (it/its)
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

cmake_minimum_required(VERSION 3.13)
project(QCVM
	DESCRIPTION "QuakeC Virtual Machine"
	HOMEPAGE_URL "https://github.com/erysdren/QCVM"
	LANGUAGES C
	VERSION 1.0.0
)

# options

option(QCVM_BUILD_EXAMPLES "Build Example Applications" ON)
option(QCVM_BUILD_QCPONG "Build Example Game QCPONG" OFF)
option(QCVM_BUILD_QCPKG "Build QCPKG tool" OFF)
option(QCVM_NO_STDLIB "Don't use the standard library" OFF)
set(QCVM_STACK_DEPTH "32" CACHE STRING "")
set(QCVM_LOCAL_STACK_DEPTH "2048" CACHE STRING "")
if(NOT DEFINED QCVM_BIG_ENDIAN)
	if(CMAKE_C_BYTE_ORDER STREQUAL "BIG_ENDIAN")
		set(QCVM_BIG_ENDIAN TRUE)
	else()
		set(QCVM_BIG_ENDIAN FALSE)
	endif()
endif()
if(NOT DEFINED QCVM_VERSION_STR)
	set(QCVM_VERSION_STR ${CMAKE_PROJECT_VERSION})
endif()

include(CheckFunctionExists)
check_function_exists(strlen HAVE_STRLEN)
check_function_exists(strcmp HAVE_STRCMP)
if(NOT HAVE_STRLEN OR NOT HAVE_STRCMP)
	set(QCVM_NO_STDLIB ON)
endif()

# configure options header

configure_file(${PROJECT_SOURCE_DIR}/cmake/qcvmconf.h.in ${PROJECT_SOURCE_DIR}/include/qcvm/qcvmconf.h @ONLY)

# main libraries

add_library(qcvm STATIC)
add_library(qcvm::qcvm ALIAS qcvm)
target_sources(qcvm PRIVATE
	${PROJECT_SOURCE_DIR}/source/qcvm.c
)

target_include_directories(qcvm
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>
)

# install

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_file(${PROJECT_SOURCE_DIR}/cmake/qcvm.pc.in ${PROJECT_BINARY_DIR}/qcvm.pc @ONLY)
write_basic_package_version_file(${PROJECT_BINARY_DIR}/qcvm-config-version.cmake COMPATIBILITY AnyNewerVersion)

install(FILES ${PROJECT_BINARY_DIR}/qcvm.pc DESTINATION lib/pkgconfig)
install(TARGETS qcvm EXPORT qcvm-targets INCLUDES DESTINATION include)
install(EXPORT qcvm-targets DESTINATION lib/cmake NAMESPACE qcvm:: FILE qcvm-config.cmake)
install(FILES ${PROJECT_BINARY_DIR}/qcvm-config-version.cmake DESTINATION lib/cmake)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/" TYPE INCLUDE FILES_MATCHING PATTERN "*.h")

# examples and utilities

if(QCVM_BUILD_QCPKG)
	add_executable(qcpkg ${PROJECT_SOURCE_DIR}/source/qcpkg.c)
	target_link_libraries(qcpkg PRIVATE qcvm qclib)
endif()

if(QCVM_BUILD_EXAMPLES OR QCVM_BUILD_QCPONG)
	find_program(QCC NAMES fteqcc fteqcc64)
	if(NOT QCC)
		message(WARNING "Can't find QuakeC compiler (fteqcc), cannot build example applications")
	endif()
endif()

if(QCVM_BUILD_QCPONG)
	find_package(SDL2)
	if(NOT SDL2_FOUND)
		message(WARNING "Can't find SDL2, cannot build example game")
	endif()
endif()

function(qcvm_build_progs tgt src dst)
	cmake_path(REMOVE_FILENAME src OUTPUT_VARIABLE srcpath)
	add_custom_command(
		OUTPUT ${dst}
		COMMAND ${QCC} ${src} -o ${dst}
		WORKING_DIRECTORY ${srcpath}
	)
	add_custom_target(${tgt} ALL DEPENDS ${dst})
endfunction()

if(QCVM_BUILD_EXAMPLES AND QCC)
	qcvm_build_progs(hello_dat ${PROJECT_SOURCE_DIR}/examples/hello/hello.qc ${PROJECT_BINARY_DIR}/hello.dat)
	add_executable(hello ${PROJECT_SOURCE_DIR}/examples/hello/hello.c)
	target_link_libraries(hello PRIVATE qcvm)
	qcvm_build_progs(sieve_dat ${PROJECT_SOURCE_DIR}/examples/sieve/sieve.qc ${PROJECT_BINARY_DIR}/sieve.dat)
	add_executable(sieve ${PROJECT_SOURCE_DIR}/examples/sieve/sieve.c)
	target_link_libraries(sieve PRIVATE qcvm)
endif()

if(QCVM_BUILD_QCPONG AND SDL2_FOUND AND QCC)
	qcvm_build_progs(qcpong_dat ${PROJECT_SOURCE_DIR}/examples/qcpong/qcpong.src ${PROJECT_BINARY_DIR}/qcpong.dat)
	add_executable(qcpong
		${PROJECT_SOURCE_DIR}/examples/qcpong/main.c
		${PROJECT_SOURCE_DIR}/examples/qcpong/stb_image_write.c
	)
	target_link_libraries(qcpong PRIVATE qcvm qclib)
	target_link_libraries(qcpong PRIVATE ${SDL2_LIBRARIES})
	target_include_directories(qcpong PRIVATE ${SDL2_INCLUDE_DIRS})
endif()
