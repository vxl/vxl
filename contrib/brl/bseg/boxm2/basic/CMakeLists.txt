# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(boxm2_basic_sources
    boxm2_array_1d.h      boxm2_array_1d.txx
    boxm2_array_2d.h      boxm2_array_2d.txx
    boxm2_array_3d.h      boxm2_array_3d.txx
    boxm2_block_id.h      boxm2_block_id.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_basic_sources)

ADD_LIBRARY(boxm2_basic  ${boxm2_basic_sources})
TARGET_LINK_LIBRARIES(boxm2_basic brdb baio expatpp vpgl vgl_xio vgl vnl vbl vsl vcl)

#install the .h .txx and libs
INSTALL_TARGETS(/lib boxm2_basic)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/basic ${boxm2_basic_sources})

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
