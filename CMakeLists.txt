cmake_minimum_required(VERSION 3.3)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

project(locc)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-D_GLIBCXX_DEBUG)
	endif()
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.?pp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_executable(locc ${SOURCES})
target_compile_features(locc PUBLIC cxx_std_17)
target_include_directories(locc PRIVATE src)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(locc PRIVATE pthread stdc++fs)
	target_compile_options(locc PRIVATE -fPIC)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(locc PRIVATE /MP)
	target_compile_definitions(locc PRIVATE _UNICODE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(locc PRIVATE -Wextra -Wall -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()
