cmake_minimum_required(VERSION 3.24)

set(package_name locc)
set(package_version 0.5.2)
project(${package_name} VERSION ${package_version})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

option(LOCC_BUILD_CLI "Build locc CLI app" ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(ext)
add_subdirectory(lib)

if(LOCC_BUILD_CLI)
  add_subdirectory(cli)
endif()
