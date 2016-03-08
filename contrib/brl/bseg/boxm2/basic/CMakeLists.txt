# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(boxm2_basic_sources
    boxm2_array_1d.h      boxm2_array_1d.hxx
    boxm2_array_2d.h      boxm2_array_2d.hxx
    boxm2_array_3d.h      boxm2_array_3d.hxx
    boxm2_block_id.h      boxm2_block_id.cxx
   )

aux_source_directory(Templates boxm2_basic_sources)

vxl_add_library(LIBRARY_NAME boxm2_basic LIBRARY_SOURCES  ${boxm2_basic_sources})
target_link_libraries(boxm2_basic brdb baio expatpp ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

#install the .h .hxx and libs

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
