# boxm's opencl Process Library
 ADD_DEFINITIONS(-DHAS_OPENCL=1)

# bprb depends on expat and boxm_ocl_pro depends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/bprb )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  
  SET(boxm_ocl_pro_sources
      boxm_ocl_processes.h
      boxm_ocl_register.h   boxm_ocl_register.cxx
  )

  AUX_SOURCE_DIRECTORY(processes boxm_ocl_pro_sources)
  AUX_SOURCE_DIRECTORY(Templates boxm_ocl_pro_sources)
 
  ADD_LIBRARY(boxm_ocl_pro ${boxm_ocl_pro_sources} )

  TARGET_LINK_LIBRARIES(boxm_ocl_pro bbas_pro boxm_ocl boxm boxm_sample bprb brdb brip sdet vsol vil_io vil)

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
