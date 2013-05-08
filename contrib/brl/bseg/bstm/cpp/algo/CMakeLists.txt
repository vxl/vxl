INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

SET(bstm_cpp_algo_sources
    bstm_data_similarity_traits.h
    bstm_ingest_boxm2_scene_function.h bstm_ingest_boxm2_scene_function.txx
    bstm_analyze_coherency_function.h bstm_analyze_coherency_function.cxx 
    bstm_label_bb_function.h)

AUX_SOURCE_DIRECTORY(Templates bstm_cpp_algo_sources)

ADD_LIBRARY(bstm_cpp_algo  ${bstm_cpp_algo_sources})
TARGET_LINK_LIBRARIES(bstm_cpp_algo bstm_basic boxm2 boxm2_cpp_algo bstm bstm_io vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
