################################################################################
##
## MIT License
##
## Copyright (c) 2023-2024 erysdren (it/she)
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
	VERSION 0.0.1
)

# options

option(QCVM_BUILD_EXAMPLES "Build Example Applications" ON)
option(QCVM_BUILD_QCPONG "Build Example Game QCPONG" ON)
option(QCVM_BUILD_QCPKG "Build QCPKG tool" ON)

# main libraries

set(QCVM_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_bootstrap.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_entities.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_exports.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_fields.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_functions.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_globals.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_opcodes.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_parameters.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_return.c
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm_runtime.c
)

set(QCVM_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/qcvm/qcvm.h
	${CMAKE_CURRENT_SOURCE_DIR}/qclib/qclib.h
)

add_library(qcvm ${QCVM_SOURCES})
target_include_directories(qcvm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/qcvm)

add_library(qclib ${CMAKE_CURRENT_SOURCE_DIR}/qclib/qclib.c)
target_link_libraries(qclib PUBLIC qcvm)
target_include_directories(qclib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/qclib)

# install

include(GNUInstallDirs)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/qcvm.pc.in ${CMAKE_CURRENT_BINARY_DIR}/qcvm.pc @ONLY)

install(TARGETS qcvm qclib DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${QCVM_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qcvm)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qcvm.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)

# examples and utilities

if(QCVM_BUILD_QCPKG)
	add_executable(qcpkg ${CMAKE_CURRENT_SOURCE_DIR}/qcpkg/qcpkg.c)
	target_link_libraries(qcpkg PRIVATE qcvm qclib)
endif()

if(QCVM_BUILD_EXAMPLES OR QCVM_BUILD_QCPONG)
	find_program(QCC fteqcc)
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
	add_custom_command(
		OUTPUT ${dst}
		COMMAND ${QCC} ${src} -o ${dst}
	)
	add_custom_target(${tgt} ALL DEPENDS ${dst})
endfunction()

if(QCVM_BUILD_EXAMPLES AND QCC)
	qcvm_build_progs(test_dat ${CMAKE_CURRENT_SOURCE_DIR}/examples/qcvm_test/test.src ${CMAKE_CURRENT_BINARY_DIR}/test.dat)
	add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/examples/qcvm_test/main.c)
	target_link_libraries(test PRIVATE qcvm qclib)

	qcvm_build_progs(test2_dat ${CMAKE_CURRENT_SOURCE_DIR}/examples/qcvm_test_fte/test2.src ${CMAKE_CURRENT_BINARY_DIR}/test2.dat)
	add_executable(test2 ${CMAKE_CURRENT_SOURCE_DIR}/examples/qcvm_test_fte/main.c)
	target_link_libraries(test2 PRIVATE qcvm qclib)

	qcvm_build_progs(module1_dat ${CMAKE_CURRENT_SOURCE_DIR}/examples/module_test/module1.src ${CMAKE_CURRENT_BINARY_DIR}/module1.dat)
	qcvm_build_progs(module2_dat ${CMAKE_CURRENT_SOURCE_DIR}/examples/module_test/module2.src ${CMAKE_CURRENT_BINARY_DIR}/module2.dat)
	add_executable(module_test ${CMAKE_CURRENT_SOURCE_DIR}/examples/module_test/module_test.c)
	target_link_libraries(module_test PRIVATE qcvm qclib)
endif()

if(QCVM_BUILD_QCPONG AND SDL2_FOUND AND QCC)
	qcvm_build_progs(qcpong_dat ${CMAKE_CURRENT_SOURCE_DIR}/examples/qcpong/qcpong.src ${CMAKE_CURRENT_BINARY_DIR}/qcpong.dat)
	add_executable(qcpong
		${CMAKE_CURRENT_SOURCE_DIR}/examples/qcpong/main.c
		${CMAKE_CURRENT_SOURCE_DIR}/examples/qcpong/stb_image_write.c
	)
	target_link_libraries(qcpong PRIVATE qcvm qclib)
	target_link_libraries(qcpong PRIVATE ${SDL2_LIBRARIES})
	target_include_directories(qcpong PRIVATE ${SDL2_INCLUDE_DIRS})
endif()
