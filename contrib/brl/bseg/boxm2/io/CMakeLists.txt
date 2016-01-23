# contrib/brl/bseg/boxm/CMakeLists.txt
include_directories( ${GEL_INCLUDE_DIR} )
include_directories( ${GEL_INCLUDE_DIR}/mrc )

include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg )

include( ${BRL_CMAKE_DIR}/FindHadoop.cmake )
if(HDFS_FOUND)
  add_definitions(-DHAS_HDFS=1)
  include_directories( ${HDFS_INCLUDE_DIRS} )
endif(HDFS_FOUND)

set(boxm2_io_sources
    boxm2_asio_mgr.h       boxm2_asio_mgr.cxx
    boxm2_sio_mgr.h        boxm2_sio_mgr.cxx
    boxm2_cache.h          boxm2_cache.cxx
    boxm2_dumb_cache.h     boxm2_dumb_cache.cxx
    boxm2_nn_cache.h       boxm2_nn_cache.cxx
    boxm2_lru_cache.h      boxm2_lru_cache.cxx
    boxm2_stream_cache.h   boxm2_stream_cache.cxx boxm2_stream_cache.txx
    boxm2_stream_block_cache.h   boxm2_stream_block_cache.cxx
    boxm2_stream_scene_cache.h   boxm2_stream_scene_cache.cxx

    boxm2_cache1.h          boxm2_cache1.cxx
    boxm2_lru_cache1.h      boxm2_lru_cache1.cxx
   )
aux_source_directory(Templates boxm2_io_sources)

add_library(boxm2_io  ${boxm2_io_sources})
target_link_libraries(boxm2_io boxm2 expatpp vpgl baio vpgl vgl_xio vgl vbl vul vsl vcl)

if(HDFS_FOUND)
 target_link_libraries(boxm2_io bhdfs)
endif(HDFS_FOUND)

#install the .h .txx and libs
install_targets(/lib boxm2_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/io ${boxm2_io_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
