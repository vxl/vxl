# acal/io/CMakeLists.txt

set(acal_io_sources
    acal_io_f_utils.h      acal_io_f_utils.cxx
    acal_io_match_graph.h  acal_io_match_graph.cxx
    acal_io_match_tree.h   acal_io_match_tree.cxx
    acal_io_match_utils.h  acal_io_match_utils.cxx
)

aux_source_directory(Templates acal_io_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}acal_io
                LIBRARY_SOURCES ${acal_io_sources}
                HEADER_INSTALL_DIR acal/io)

target_link_libraries(${VXL_LIB_PREFIX}acal_io ${VXL_LIB_PREFIX}acal ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vsl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
