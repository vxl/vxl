# brl/bbas/bjson/CMakeLists.txt
# Parse JSON files

set(bjson_sources
 bjson.h bjsoncpp.cxx
)
vxl_add_library(LIBRARY_NAME bjson LIBRARY_SOURCES  ${bjson_sources})

target_link_libraries(bjson)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
