# bvxm_algo Process Library

# bprb depends on expat and this process library depends on bprb
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
 IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES( ${EXPATPP_INCLUDE_DIRS} )

  SET(bvxm_algo_pro_sources
      bvxm_algo_processes.h
      bvxm_algo_register.h   bvxm_algo_register.cxx
     )
  AUX_SOURCE_DIRECTORY(processes bvxm_algo_pro_sources)

  ADD_LIBRARY(bvxm_algo_pro ${bvxm_algo_pro_sources} )
  TARGET_LINK_LIBRARIES(bvxm_algo_pro bvxm_algo bvxm_grid_pro boxm boct bprb brdb vul vbl)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)
