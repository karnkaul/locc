cmake_minimum_required(VERSION 3.23)

project(locc VERSION "0.4.0")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

set(not_top_level ON)

if(${PROJECT_IS_TOP_LEVEL})
  set(not_top_level OFF)
endif()

set(FETCHCONTENT_QUIET ${not_top_level})

option(LOCC_BUILD_CLI "Build locc CLI app" ${PROJECT_IS_TOP_LEVEL})
option(LOCC_BUILD_GUI "Build locc GUI app" ${PROJECT_IS_TOP_LEVEL})
option(LOCC_BUILD_TESTS "Build locc tests" ${PROJECT_IS_TOP_LEVEL})

add_library(${PROJECT_NAME}-compile-options INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME}-compile-options ALIAS ${PROJECT_NAME}-compile-options)

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  target_compile_options(${PROJECT_NAME}-compile-options INTERFACE
    -Wall -Wextra -Wpedantic -Wconversion -Werror=return-type
  )
endif()

include(FetchContent)
set(ext_dir "${CMAKE_CURRENT_SOURCE_DIR}/ext")
set(fetch_targets "")

if(LOCC_BUILD_GUI AND NOT TARGET gvdi)
  FetchContent_Declare(gvdi
    GIT_REPOSITORY https://github.com/karnkaul/gvdi
    GIT_TAG v0.2.3
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/gvdi"
  )
  list(APPEND fetch_targets gvdi)
endif()

if(NOT TARGET djson)
  FetchContent_Declare(djson
    GIT_REPOSITORY https://github.com/karnkaul/djson
    GIT_TAG v2.1.2
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/djson"
  )
  list(APPEND fetch_targets djson)
endif()

if(NOT TARGET klib)
  FetchContent_Declare(klib
    GIT_REPOSITORY https://github.com/karnkaul/klib
    GIT_TAG v0.3.2
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/klib"
  )
  list(APPEND fetch_targets klib)
endif()

FetchContent_MakeAvailable(${fetch_targets})

add_subdirectory(lib)

if(LOCC_BUILD_CLI)
  add_subdirectory(cli)
endif()

if(LOCC_BUILD_GUI)
  add_subdirectory(gui)
endif()

if(LOCC_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
