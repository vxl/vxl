# boxm's opencl Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  
  SET(boxm_opt_opencl_pro_sources
      boxm_opt_opencl_processes.h
      boxm_opt_opencl_register.h   boxm_opt_opencl_register.cxx
  )

  AUX_SOURCE_DIRECTORY(processes boxm_opt_opencl_pro_sources)

  ADD_LIBRARY(boxm_opt_opencl_pro ${boxm_opt_opencl_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm_opt_opencl_pro boxm_opt_opencl boxm_opt boxm_io bprb brdb brip sdet vsol vil)
ENDIF(EXPAT_FOUND)
