# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(boxm2_basic_sources
    boxm2_array_1d.h      boxm2_array_1d.txx
    boxm2_array_2d.h      boxm2_array_2d.txx
    boxm2_array_3d.h      boxm2_array_3d.txx
    boxm2_block_id.h      boxm2_block_id.cxx
   )

aux_source_directory(Templates boxm2_basic_sources)

add_library(boxm2_basic  ${boxm2_basic_sources})
target_link_libraries(boxm2_basic brdb baio expatpp vpgl vgl_xio vgl vnl vbl vsl vcl)

#install the .h .txx and libs
install_targets(/lib boxm2_basic)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/basic ${boxm2_basic_sources})

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
