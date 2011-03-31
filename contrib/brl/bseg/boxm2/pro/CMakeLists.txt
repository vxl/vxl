# boxm2's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
 IF(lems_SOURCE_DIR)
  INCLUDE_DIRECTORIES( ${lems_SOURCE_DIR}/contrib/dec )
 ENDIF(lems_SOURCE_DIR)

  SET(boxm2_pro_sources
      boxm2_processes.h
      boxm2_register.h   boxm2_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates boxm2_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm2_pro_sources)

  ADD_LIBRARY(boxm2_pro ${boxm2_pro_sources})

  TARGET_LINK_LIBRARIES(boxm2_pro boxm2 bvgl sdet bprb brdb brip imesh vsol bsta bsta_pro vnl vil vgl vbl_io vul)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
