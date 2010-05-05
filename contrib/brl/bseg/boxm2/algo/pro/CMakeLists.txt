# boxm2's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  SET(boxm2_algo_pro_sources
      boxm_algo_processes.h
      boxm_algo_register.h   boxm_algo_register.cxx
  )

  #AUX_SOURCE_DIRECTORY(Templates boxm2_algo_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm2_algo_pro_sources)

  ADD_LIBRARY(boxm2_algo_pro ${boxm2_algo_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm2_algo_pro boxm2_algo boxm2 boxm2_sample boxm2_util boct bprb brdb bsta vpgl bgeo vpgl_algo vil vsol vgl brip vcl)


ENDIF(EXPAT_FOUND)
