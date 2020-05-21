# acal/io/CMakeLists.txt

set(acal_io_sources
    acal_io_f_utils.h      acal_io_f_utils.cxx
    acal_io_match_graph.h  acal_io_match_graph.cxx
    acal_io_match_utils.h  acal_io_match_utils.cxx
)

vxl_add_library(LIBRARY_NAME acal_io LIBRARY_SOURCES ${acal_io_sources})

target_link_libraries(acal_io ${VXL_LIB_PREFIX}acal ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
