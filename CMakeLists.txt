cmake_minimum_required(VERSION 2.8)
project(lubee)
include(aux.cmake)

set(CXX_VERSION 17)
include(cxx_options.cmake)
string(CONCAT CMAKE_CXX_FLAGS " -Wno-c99-extensions")

if(NOT without-test)
	enable_testing()
	find_package(Threads REQUIRED)
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})
	# testsディレクトリ以下のソースが対象
	aux_source_directory(tests TEST_SRC)
	# 個別にexeを作成
	foreach(SRC IN LISTS TEST_SRC)
		GetFileName(${SRC}, SRCNAME)
		AddTest(${SRCNAME} ${SRC})
	endforeach()
endif()
