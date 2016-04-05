# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

include( ${BRL_CMAKE_DIR}/FindHadoop.cmake )
if(HDFS_FOUND)
  add_definitions(-DHAS_HDFS=1)
  include_directories( ${HDFS_INCLUDE_DIRS} )
endif()

set(boxm2_io_sources
    boxm2_asio_mgr.h       boxm2_asio_mgr.cxx
    boxm2_sio_mgr.h        boxm2_sio_mgr.cxx
    boxm2_cache.h          boxm2_cache.cxx
    boxm2_dumb_cache.h     boxm2_dumb_cache.cxx
    boxm2_nn_cache.h       boxm2_nn_cache.cxx
    boxm2_lru_cache.h      boxm2_lru_cache.cxx
    boxm2_stream_cache.h   boxm2_stream_cache.cxx boxm2_stream_cache.hxx
    boxm2_stream_block_cache.h   boxm2_stream_block_cache.cxx
    boxm2_stream_scene_cache.h   boxm2_stream_scene_cache.cxx

    boxm2_cache1.h          boxm2_cache1.cxx
    boxm2_lru_cache1.h      boxm2_lru_cache1.cxx
   )
aux_source_directory(Templates boxm2_io_sources)

vxl_add_library(LIBRARY_NAME boxm2_io LIBRARY_SOURCES  ${boxm2_io_sources})
target_link_libraries(boxm2_io boxm2 expatpp ${VXL_LIB_PREFIX}vpgl baio ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl)

if(HDFS_FOUND)
 target_link_libraries(boxm2_io bhdfs)
endif()

#install the .h .hxx and libs

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
