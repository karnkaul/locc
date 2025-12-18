project(${package_name}-lib VERSION ${package_version})

add_library(${PROJECT_NAME})
add_library(${package_name}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)

target_link_libraries(${PROJECT_NAME} PUBLIC
  klib::klib
  djson::djson
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
)

set(version_header include/${package_name}/build_version.hpp)
configure_file(src/build_version.hpp.in ${version_header} @ONLY)

file(GLOB_RECURSE headers LIST_DIRECTORIES false "include/*.hpp")

target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS
  BASE_DIRS
  include
  "${CMAKE_CURRENT_BINARY_DIR}/include"

  FILES
  ${headers}
  "${CMAKE_CURRENT_BINARY_DIR}/${version_header}"
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "src/*.[hc]pp")

target_sources(${PROJECT_NAME} PRIVATE
  ${sources}
)
