cmake_minimum_required(VERSION 3.3)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

set(LOCC_VERSION "0.0.4" CACHE STRING "Application Version" FORCE)
project(locc VERSION ${LOCC_VERSION})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_definitions(-D_GLIBCXX_DEBUG)
	endif()
endif()

find_package(Git QUIET)
set(LOCC_COMMIT_HASH "[unknown]")
if(GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		OUTPUT_VARIABLE LOCC_COMMIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	message(STATUS "Updating git submodules at ${CMAKE_CURRENT_SOURCE_DIR}")
	execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		RESULT_VARIABLE UPDATE_SUBMODULES_RESULT
	)
	if(NOT UPDATE_SUBMODULES_RESULT EQUAL "0")
		message(FATAL_ERROR "git submodule update failed! ${UPDATE_SUBMODULES_RESULT}")
	endif()
else()
	message(WARNING "Git / .git not found, submodules missing, expect compile errors!")
endif()

add_subdirectory(libs/dumb_json)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.?pp")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})
set(OUTFILE "${CMAKE_CURRENT_BINARY_DIR}/src/build_version.hpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/build_version.hpp.in" "${OUTFILE}")
source_group(TREE "${CMAKE_CURRENT_BINARY_DIR}" FILES "${OUTFILE}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml.in" "${CMAKE_CURRENT_SOURCE_DIR}/.appveyor.yml")

add_executable(locc ${SOURCES})
target_compile_features(locc PUBLIC cxx_std_17)
target_include_directories(locc PRIVATE src "${CMAKE_CURRENT_BINARY_DIR}/src")
target_link_libraries(locc PRIVATE djson)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(locc PRIVATE pthread stdc++fs)
	target_compile_options(locc PRIVATE -fPIC)
	target_link_options(locc PRIVATE -no-pie)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(locc PRIVATE /MP)
	target_compile_definitions(locc PRIVATE _UNICODE)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(locc PRIVATE -Wextra -Wall -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()
