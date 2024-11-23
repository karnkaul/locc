set(exe_name ${PROJECT_NAME}-cli)

add_executable(${exe_name})

target_link_libraries(${exe_name} PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}-lib
  ${PROJECT_NAME}::${PROJECT_NAME}-compile-options
  clap::clap
)

target_include_directories(${exe_name} PRIVATE
  src
)

target_sources(${exe_name} PRIVATE
  src/app.cpp
  src/app.hpp
  src/counter.cpp
  src/counter.hpp
  src/main.cpp
  src/params.hpp
  src/table.cpp
  src/table.hpp
)
