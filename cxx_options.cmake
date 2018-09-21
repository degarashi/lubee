# ライブラリ共通で使用するcmakeのセッティングファイル
# CXX_VERSION で 11や14などを指定

enable_language(CXX)
# C++バージョンを指定
set(CMAKE_CXX_STANDARD ${CXX_VERSION})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# コンパイラ依存の拡張をオフにする
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -ftemplate-depth=1024")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	# デバッグビルドの際はGLibのデバッグモードをONにする
	add_definitions(-D_GLIBCXX_DEBUG)
	add_definitions(-DDEBUG)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
