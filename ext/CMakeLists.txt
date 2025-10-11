file(ARCHIVE_EXTRACT INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src.zip" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}")

if(NOT TARGET djson::djson)
  message(STATUS "[djson]")
  set(DJSON_INSTALL OFF)
  add_subdirectory(src/djson)
endif()

if (NOT TARGET klib::klib)
  message(STATUS "[klib]")
  set(KLIB_INSTALL OFF)
  add_subdirectory(src/klib)
endif()

if (LOCC_BUILD_GUI AND NOT TARGET gvdi::gvdi)
  message(STATUS "[gvdi]")
  add_subdirectory(src/gvdi)
endif()
