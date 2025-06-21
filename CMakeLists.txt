################################################################################
##
## MIT License
##
## Copyright (c) 2023-2025 erysdren (it/its)
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

set(QCVM_HEADERS
	${PROJECT_SOURCE_DIR}/include/qcvm.h
	${PROJECT_SOURCE_DIR}/include/qclib.h
)

add_library(qcvm STATIC)
add_library(qcvm::qcvm ALIAS qcvm)
target_sources(qcvm PRIVATE
	${PROJECT_SOURCE_DIR}/source/qcvm_bootstrap.c
	${PROJECT_SOURCE_DIR}/source/qcvm_entities.c
	${PROJECT_SOURCE_DIR}/source/qcvm_exports.c
	${PROJECT_SOURCE_DIR}/source/qcvm_fields.c
	${PROJECT_SOURCE_DIR}/source/qcvm_functions.c
	${PROJECT_SOURCE_DIR}/source/qcvm_globals.c
	${PROJECT_SOURCE_DIR}/source/qcvm_opcodes.c
	${PROJECT_SOURCE_DIR}/source/qcvm_parameters.c
	${PROJECT_SOURCE_DIR}/source/qcvm_return.c
	${PROJECT_SOURCE_DIR}/source/qcvm_runtime.c
)

target_include_directories(qcvm
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>
)

add_library(qclib STATIC)
add_library(qcvm::qclib ALIAS qclib)
target_link_libraries(qclib PRIVATE qcvm)
target_sources(qclib PRIVATE
	${PROJECT_SOURCE_DIR}/source/qclib.c
)

target_include_directories(qclib
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>
)


# install

include(CMakePackageConfigHelpers)

configure_file(${PROJECT_SOURCE_DIR}/contrib/qcvm.pc.in ${PROJECT_BINARY_DIR}/qcvm.pc @ONLY)
write_basic_package_version_file(${PROJECT_BINARY_DIR}/qcvm-config-version.cmake COMPATIBILITY AnyNewerVersion)

install(FILES ${PROJECT_BINARY_DIR}/qcvm.pc DESTINATION lib/pkgconfig)
install(TARGETS qcvm qclib EXPORT qcvm-targets INCLUDES DESTINATION include)
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
	qcvm_build_progs(test_dat ${PROJECT_SOURCE_DIR}/examples/qcvm_test/test.src ${PROJECT_BINARY_DIR}/test.dat)
	add_executable(test ${PROJECT_SOURCE_DIR}/examples/qcvm_test/main.c)
	target_link_libraries(test PRIVATE qcvm qclib)

	qcvm_build_progs(test2_dat ${PROJECT_SOURCE_DIR}/examples/qcvm_test_fte/test2.src ${PROJECT_BINARY_DIR}/test2.dat)
	add_executable(test2 ${PROJECT_SOURCE_DIR}/examples/qcvm_test_fte/main.c)
	target_link_libraries(test2 PRIVATE qcvm qclib)

	qcvm_build_progs(module1_dat ${PROJECT_SOURCE_DIR}/examples/module_test/module1.src ${PROJECT_BINARY_DIR}/module1.dat)
	qcvm_build_progs(module2_dat ${PROJECT_SOURCE_DIR}/examples/module_test/module2.src ${PROJECT_BINARY_DIR}/module2.dat)
	add_executable(module_test ${PROJECT_SOURCE_DIR}/examples/module_test/module_test.c)
	target_link_libraries(module_test PRIVATE qcvm qclib)
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
