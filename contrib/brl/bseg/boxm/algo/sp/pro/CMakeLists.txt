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

  SET(boxm_algo_sp_pro_sources
      boxm_algo_sp_processes.h
      boxm_algo_sp_register.h   boxm_algo_sp_register.cxx
     )
  AUX_SOURCE_DIRECTORY(Templates boxm_algo_sp_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm_algo_sp_pro_sources)

  ADD_LIBRARY(boxm_algo_sp_pro ${boxm_algo_sp_pro_sources} )
  TARGET_LINK_LIBRARIES(boxm_algo_sp_pro boxm boxm_algo_sp boxm_util bprb brdb brip sdet vsol vil_pro vil_io vil)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
