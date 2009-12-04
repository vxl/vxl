# boxm open_cl acceleration library

INCLUDE( ${BRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bcl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  SET(boxm_opt_opencl_sources
  boxm_ray_trace_manager.h   boxm_ray_trace_manager.cxx 
  boxm_opencl_manager.h   boxm_opencl_manager.txx 
  )

  AUX_SOURCE_DIRECTORY(Templates boxm_opt_opencl_sources)	
  ADD_LIBRARY(boxm_opt_opencl ${boxm_opt_opencl_sources})

  TARGET_LINK_LIBRARIES(boxm_opt_opencl boxm_opt boxm bprb bgeo vgl vbl vbl_io ${OPENCL_LIBRARIES})

SUBDIRS(tests)
ENDIF(OPENCL_FOUND)
