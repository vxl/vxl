# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_multi_basic_sources
    array_4d.h
   )

aux_source_directory(Templates bstm_multi_basic_sources)

vxl_add_library(LIBRARY_NAME bstm_multi_basic LIBRARY_SOURCES  ${bstm_multi_basic_sources})
target_link_libraries(bstm_multi_basic ${VXL_LIB_PREFIX}vcl)

#install the .h .hxx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
