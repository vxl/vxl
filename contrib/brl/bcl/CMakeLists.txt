# brl/bcl/CMakeLists.txt
# Platform-independent includes for Brown University Libraries

INCLUDE(${BRL_MODULE_PATH}/find_open_cl.cmake )
IF(OPENCL_FOUND)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_DIR} )

  SET(bcl_sources
  bcl_cl.h
)

ENDIF(OPENCL_FOUND)