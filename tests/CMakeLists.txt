file(GLOB_RECURSE sources LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

if(NOT "${sources}" STREQUAL "")
  add_executable(${PROJECT_NAME}-tests)
  target_link_libraries(${PROJECT_NAME}-tests PRIVATE
    ktest::ktest-main
    ${PROJECT_NAME}::${PROJECT_NAME}-lib
    ${PROJECT_NAME}::${PROJECT_NAME}-compile-options
  )
  target_include_directories(${PROJECT_NAME}-tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  target_sources(${PROJECT_NAME}-tests PRIVATE ${sources})
  add_test(NAME ${PROJECT_NAME}-tests COMMAND ${PROJECT_NAME}-tests)
endif()
