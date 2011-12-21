# bhdfs's Process Library

# bprb depends on expat and bhdfs_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
 
  SET(bhdfs_pro_sources
      bhdfs_processes.h
      bhdfs_register.h   bhdfs_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates bhdfs_pro_sources)
  AUX_SOURCE_DIRECTORY(processes bhdfs_pro_sources)

  ADD_LIBRARY(bhdfs_pro ${bhdfs_pro_sources})

  TARGET_LINK_LIBRARIES(bhdfs_pro bhdfs bprb vil_algo vil_io vil)

ENDIF(EXPAT_FOUND)
