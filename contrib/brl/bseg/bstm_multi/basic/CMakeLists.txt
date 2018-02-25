# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_multi_basic_sources
    array_4d.h
    array_4d.cxx
   )

## This does not exist aux_source_directory(Templates bstm_multi_basic_sources)

vxl_add_library(LIBRARY_NAME bstm_multi_basic LIBRARY_SOURCES  ${bstm_multi_basic_sources})
set_target_properties(bstm_multi_basic PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(bstm_multi_basic ${VXL_LIB_PREFIX}vcl)

#install the .h .hxx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
