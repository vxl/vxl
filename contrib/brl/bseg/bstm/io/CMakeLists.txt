# contrib/brl/bseg/bstm/CMakeLists.txt

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_io_sources
    bstm_cache.h          bstm_cache.cxx
    bstm_sio_mgr.h        bstm_sio_mgr.cxx
    bstm_lru_cache.h        bstm_lru_cache.cxx
   )
AUX_SOURCE_DIRECTORY(Templates bstm_io_sources)

ADD_LIBRARY(bstm_io  ${bstm_io_sources})
TARGET_LINK_LIBRARIES(bstm_io bstm bstm_basic)

#install the .h .txx and libs
INSTALL_TARGETS(/lib bstm_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/io ${bstm_io_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
