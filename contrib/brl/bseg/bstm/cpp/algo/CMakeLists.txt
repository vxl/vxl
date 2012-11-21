INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_cpp_algo_sources
    bstm_ingest_boxm2_scene_function.h bstm_ingest_boxm2_scene_function.cxx)

ADD_LIBRARY(bstm_cpp_algo  ${bstm_cpp_algo_sources})
TARGET_LINK_LIBRARIES(bstm_cpp_algo bstm_basic boxm2 boxm2_cpp_algo boxm2_io bstm vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )