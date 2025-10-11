cmake_minimum_required(VERSION 3.23)

project(locc VERSION "0.4.2")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

option(LOCC_BUILD_CLI "Build locc CLI app" ${PROJECT_IS_TOP_LEVEL})
option(LOCC_BUILD_GUI "Build locc GUI app" ${PROJECT_IS_TOP_LEVEL})
option(LOCC_BUILD_TESTS "Build locc tests" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(ext)
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
