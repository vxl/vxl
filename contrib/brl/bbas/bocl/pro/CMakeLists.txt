# bprb depends on expat and boxm_ocl_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  
  SET(bocl_pro_sources
      bocl_processes.h
      bocl_register.h   bocl_register.cxx
  )

  AUX_SOURCE_DIRECTORY(processes bocl_pro_sources)
  AUX_SOURCE_DIRECTORY(Templates bocl_pro_sources)
 
  ADD_LIBRARY(bocl_pro ${bocl_pro_sources} )

  TARGET_LINK_LIBRARIES(bocl_pro bocl bprb brdb )

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
