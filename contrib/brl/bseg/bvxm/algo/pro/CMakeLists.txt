# bvxm_algo Process Library

# bprb depends on expat and this process library depends on bprb
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

  SET(bvxm_algo_pro_sources
      bvxm_algo_processes.h
      bvxm_algo_register.h   bvxm_algo_register.cxx
  )

  AUX_SOURCE_DIRECTORY(processes bvxm_algo_pro_sources)

  ADD_LIBRARY(bvxm_algo_pro ${bvxm_algo_pro_sources} )

  TARGET_LINK_LIBRARIES(bvxm_algo_pro bvxm_algo bprb brdb)
ENDIF(EXPAT_FOUND)
