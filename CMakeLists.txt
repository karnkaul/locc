cmake_minimum_required(VERSION 3.23)

project(locc VERSION "0.3.0")

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

if(LOCC_BUILD_CLI AND NOT TARGET clap)
  set(CLAP_BUILD_TESTS ${PROJECT_IS_TOP_LEVEL})
  set(CLAP_BUILD_EXAMPLES ${PROJECT_IS_TOP_LEVEL})
  FetchContent_Declare(clap
    GIT_REPOSITORY https://github.com/karnkaul/clap
    GIT_TAG v0.5.0
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/clap"
  )
  list(APPEND fetch_targets clap)
endif()

if(LOCC_BUILD_GUI AND NOT TARGET gvdi)
  FetchContent_Declare(gvdi
    GIT_REPOSITORY https://github.com/karnkaul/gvdi
    GIT_TAG v0.2.3
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/gvdi"
  )
  list(APPEND fetch_targets gvdi)
endif()

if(LOCC_BUILD_TESTS AND NOT TARGET ktest)
  FetchContent_Declare(ktest
    GIT_REPOSITORY https://github.com/karnkaul/ktest
    GIT_TAG v0.1.1
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/ktest"
  )
  list(APPEND fetch_targets ktest)
endif()

if(NOT TARGET ktask)
  set(KTASK_BUILD_TESTS ${PROJECT_IS_TOP_LEVEL})
  FetchContent_Declare(ktask
    GIT_REPOSITORY https://github.com/karnkaul/ktask
    GIT_TAG v0.2.2
    GIT_SHALLOW TRUE
    SOURCE_DIR "${ext_dir}/src/ktask"
  )
  list(APPEND fetch_targets ktask)
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
