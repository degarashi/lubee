cmake_minimum_required(VERSION 2.8)
project(lubee)
include(aux.cmake)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -ggdb3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++14 -Wall -Wextra -ftemplate-depth=1024")
add_definitions(-DSSE=${SSE})
# デバッグビルドの際はGLibのデバッグモードをONにする
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-D_GLIBCXX_DEBUG)
endif()

if(NOT without-test)
	enable_testing()
	find_package(Threads REQUIRED)
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})
	# testsディレクトリ以下のソースが対象
	aux_source_directory(tests TEST_SRC)
	AddTest(test "${TEST_SRC}")
endif()