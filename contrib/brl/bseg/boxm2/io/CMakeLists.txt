# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(boxm2_io_sources
    boxm2_asio_mgr.h       boxm2_asio_mgr.cxx
    boxm2_sio_mgr.h        boxm2_sio_mgr.cxx
    boxm2_cache.h          boxm2_cache.cxx
    boxm2_dumb_cache.h     boxm2_dumb_cache.cxx
    boxm2_nn_cache.h       boxm2_nn_cache.cxx
    boxm2_lru_cache.h      boxm2_lru_cache.cxx
    boxm2_scene_parser.h   boxm2_scene_parser.cxx
   )

ADD_LIBRARY(boxm2_io  ${boxm2_io_sources})
TARGET_LINK_LIBRARIES(boxm2_io boxm2 expatpp vpgl baio vpgl vgl_xio vgl vbl vul vsl vcl)
