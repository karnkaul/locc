set(lib_name ${PROJECT_NAME}-lib)

add_library(${lib_name})
add_library(${PROJECT_NAME}::${lib_name} ALIAS ${lib_name})

target_link_libraries(${lib_name} PUBLIC
  klib::klib
  djson::djson
)

target_compile_features(${lib_name} PUBLIC
  cxx_std_23
)

target_include_directories(${lib_name} PRIVATE
  src
)

target_sources(${lib_name} PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
  include/locc/count_lines.hpp
  include/locc/file_filter.hpp
  include/locc/grammar.hpp
  include/locc/instance.hpp
  include/locc/json_io.hpp
  include/locc/line_count.hpp
  include/locc/line_counter.hpp
  include/locc/progress.hpp
  include/locc/query.hpp
  include/locc/row.hpp
)

target_sources(${lib_name} PRIVATE
  src/count_lines.cpp
  src/file_filter.cpp
  src/grammar.cpp
  src/instance.cpp
  src/json_io.cpp
  src/line_counter.cpp
  src/loc_type.hpp
  src/lut.cpp
  src/lut.hpp
  src/row.cpp
  src/scanner.cpp
  src/scanner.hpp
  src/util.hpp
)

get_target_property(sources ${lib_name} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})
configure_file(src/build_version.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/build_version.hpp")
source_group("include/${PROJECT_NAME}" FILES "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/build_version.hpp")

target_sources(${lib_name} PUBLIC FILE_SET HEADERS BASE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/include" FILES
  "${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}/build_version.hpp"
)
