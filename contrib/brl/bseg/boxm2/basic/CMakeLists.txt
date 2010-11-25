# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(boxm2_basic_sources
    boxm2_array_1d.h                boxm2_array_1d.txx
    boxm2_array_2d.h                boxm2_array_2d.txx
    boxm2_array_3d.h                boxm2_array_3d.txx
   )

AUX_SOURCE_DIRECTORY(Templates boxm2_basic_sources)

ADD_LIBRARY(boxm2_basic  ${boxm2_basic_sources})
TARGET_LINK_LIBRARIES(boxm2_basic expatpp vpgl vgl vgl_xio vbl vsl vcl baio)

#tests
SUBDIRS(tests)
