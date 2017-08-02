# contrib/brl/bseg/bstm/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_multi_io_sources
    space_time_cache.h
    space_time_sio_mgr.h        space_time_sio_mgr.hxx
    # space_time_lru_cache.h        space_time_lru_cache.cxx
   )
aux_source_directory(Templates bstm_multi_io_sources)

vxl_add_library(LIBRARY_NAME bstm_multi_io LIBRARY_SOURCES  ${bstm_multi_io_sources})
target_link_libraries(bstm_multi_io bstm_multi)

#install the .h .hxx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
