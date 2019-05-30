cmake_minimum_required(VERSION 2.8)
project(lubee)

set(COMMON_COMPILE common_compile)
include(${COMMON_COMPILE}/cxx_setup.cmake)
include(${COMMON_COMPILE}/compile_commands.cmake)
include(${COMMON_COMPILE}/aux.cmake)
SetupCXX(17 CompileCommands)
string(CONCAT CMAKE_CXX_FLAGS " -Wno-c99-extensions")

if(NOT without-test)
	# コンパイル出来るかだけのチェック
	aux_source_directory(src S_SRC)
	add_executable(StaticTest ${S_SRC})

	enable_testing()
	# testsディレクトリ以下のソースが対象
	aux_source_directory(src/tests TEST_SRC)
	LoadGTestLibs(LIBS)
	# 個別にexeを作成
	foreach(SRC IN LISTS TEST_SRC)
		GetFileName(${SRC}, SRCNAME)
		AddTest(${PROJECT_NAME} ${SRCNAME} ${SRC} "${LIBS}")
	endforeach()
	DefineCompDB(TEST_SRC)
endif()
