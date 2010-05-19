# boxm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
 IF(lems_SOURCE_DIR)
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
 ENDIF(lems_SOURCE_DIR)

  SET(boxm_algo_rt_pro_sources
      boxm_algo_rt_processes.h
      boxm_algo_rt_register.h   boxm_algo_rt_register.cxx
  )

  #AUX_SOURCE_DIRECTORY(Templates boxm_algo_rt_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm_algo_rt_pro_sources)

  ADD_LIBRARY(boxm_algo_rt_pro ${boxm_algo_rt_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm_algo_rt_pro boxm_algo_rt boxm boxm_sample boxm_sample boct bprb brdb brip sdet vil vcl)


ENDIF(EXPAT_FOUND)
