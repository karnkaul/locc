set(exe_name ${PROJECT_NAME}-gui)

add_executable(${exe_name})

target_link_libraries(${exe_name} PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}-lib
  gvdi::gvdi
)

target_include_directories(${exe_name} PRIVATE
  src
)

target_sources(${exe_name} PRIVATE
  src/app.cpp
  src/app.hpp
  src/config.cpp
  src/config.hpp
  src/counter.cpp
  src/counter.hpp
  src/main.cpp
)
