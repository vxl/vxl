# This is contrib/brl/bseg/bvpl/pro/CMakeLists.txt
# bvpl's Process Library

# bprb depends on expat and bvpl_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
  SET(bvpl_pro_sources
      bvpl_processes.h
      bvpl_register.h    bvpl_register.cxx
     )

  AUX_SOURCE_DIRECTORY(Templates bvpl_pro_sources)
  AUX_SOURCE_DIRECTORY(processes bvpl_pro_sources)
  AUX_SOURCE_DIRECTORY(processes/util bvpl_pro_sources)

  ADD_LIBRARY(bvpl_pro ${bvpl_pro_sources})

  TARGET_LINK_LIBRARIES(bvpl_pro bprb brdb bvpl bvpl_util bvpl_util_io bvpl_kernels_pro bvrml bwm_video vil_algo vil vgl vnl vul vbl_io)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
