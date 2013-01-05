# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_basic_sources
    bstm_block_id.h      bstm_block_id.cxx
   )

AUX_SOURCE_DIRECTORY(Templates bstm_basic_sources)

ADD_LIBRARY(bstm_basic  ${bstm_basic_sources})
TARGET_LINK_LIBRARIES(bstm_basic vcl)

#install the .h .txx and libs
INSTALL_TARGETS(/lib bstm_basic)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/basic ${bstm_basic_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
