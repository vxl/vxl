# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_basic_sources
    bstm_block_id.h      bstm_block_id.cxx
   )

aux_source_directory(Templates bstm_basic_sources)

add_library(bstm_basic  ${bstm_basic_sources})
target_link_libraries(bstm_basic ${VXL_LIB_PREFIX}vcl)

#install the .h .txx and libs
install_targets(/lib bstm_basic)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/basic ${bstm_basic_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
