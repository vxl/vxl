include_directories( ${BRL_INCLUDE_DIR}/bseg )

set(bstm_multi_cpp_algo_sources
    bstm_multi_bstm_block_to_bstm_multi_block_function.h bstm_multi_bstm_block_to_bstm_multi_block_function.cxx)

aux_source_directory(Templates bstm_multi_cpp_algo_sources)

vxl_add_library(LIBRARY_NAME bstm_multi_cpp_algo LIBRARY_SOURCES  ${bstm_multi_cpp_algo_sources})
target_link_libraries(bstm_multi_cpp_algo boxm2 boxm2_cpp_algo bstm bstm_multi bstm_multi_io ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
