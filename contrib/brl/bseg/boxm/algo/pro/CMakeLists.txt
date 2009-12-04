# boxm_algo Process Library

# bprb depends on expat and this process library depends on bprb
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )

  SET(boxm_algo_pro_sources
      boxm_algo_processes.h
      boxm_algo_register.h   boxm_algo_register.cxx
  )

  #AUX_SOURCE_DIRECTORY(Templates boxm_algo_pro_sources)
  AUX_SOURCE_DIRECTORY(processes boxm_algo_pro_sources)

  ADD_LIBRARY(boxm_algo_pro ${boxm_algo_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm_algo_pro boxm_algo boxm_io bprb brdb)
ENDIF(EXPAT_FOUND)
