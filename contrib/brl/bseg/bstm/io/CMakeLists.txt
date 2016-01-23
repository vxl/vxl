# contrib/brl/bseg/bstm/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_io_sources
    bstm_cache.h          bstm_cache.cxx
    bstm_sio_mgr.h        bstm_sio_mgr.cxx
    bstm_lru_cache.h        bstm_lru_cache.cxx
   )
aux_source_directory(Templates bstm_io_sources)

add_library(bstm_io  ${bstm_io_sources})
target_link_libraries(bstm_io bstm bstm_basic)

#install the .h .txx and libs
install_targets(/lib bstm_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/io ${bstm_io_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
