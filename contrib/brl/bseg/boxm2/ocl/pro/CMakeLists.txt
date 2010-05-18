# boxm's opencl Process Library

# bprb depends on expat and boxm_ocl_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  
  SET(boxm2_ocl_pro_sources
      boxm_ocl_processes.h
      boxm_ocl_register.h   boxm_ocl_register.cxx
  )

  AUX_SOURCE_DIRECTORY(processes boxm2_ocl_pro_sources)

  ADD_LIBRARY(boxm2_ocl_pro ${boxm2_ocl_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm2_ocl_pro boxm2_ocl boxm2 boxm2_sample bprb brdb brip sdet vsol vil)
ENDIF(EXPAT_FOUND)
