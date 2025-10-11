set(package_name ${PROJECT_NAME}-cli)

add_executable(${package_name})

target_link_libraries(${package_name} PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}-lib
)

target_include_directories(${package_name} PRIVATE
  src
)

target_sources(${package_name} PRIVATE
  src/app.cpp
  src/app.hpp
  src/counter.cpp
  src/counter.hpp
  src/main.cpp
  src/params.hpp
  src/table.cpp
  src/table.hpp
)

message(STATUS "[${package_name}] setting up install")
include(GNUInstallDirs)
install(TARGETS ${package_name})
