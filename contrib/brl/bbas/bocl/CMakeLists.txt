# basic open_cl library

INCLUDE( ${BRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )

  SET(bocl_sources
      bocl_manager.h    bocl_manager.txx
      bocl_cl.h
  )

ENDIF(OPENCL_FOUND)
