cmake_minimum_required(VERSION 3.3)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

project(loc)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-D_GLIBCXX_DEBUG)
	endif()
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.?pp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
add_executable(loc ${SOURCES})
target_compile_features(loc PUBLIC cxx_std_17)
target_include_directories(loc PRIVATE src)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(loc PRIVATE pthread stdc++fs)
	target_compile_options(loc PRIVATE -fPIC)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(loc PRIVATE -Wextra -Wall $<$<NOT:$<CONFIG:Debug>>:-Werror> -Werror=return-type)
endif()
