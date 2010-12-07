# basic open_cl library

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

  SET(bocl_sources
      bocl_manager.h    bocl_manager.txx
      bocl_cl.h
      bocl_cl_gl.h
      bocl_global_memory_bandwidth_manager.h bocl_global_memory_bandwidth_manager.cxx
      bocl_utils.h      bocl_utils.cxx
      bocl_mem.h        bocl_mem.cxx
      bocl_kernel.h     bocl_kernel.cxx
  )
  AUX_SOURCE_DIRECTORY(Templates bocl_sources)
  ADD_LIBRARY(bocl ${bocl_sources} )

  TARGET_LINK_LIBRARIES(bocl vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES})

#tests
SUBDIRS(tests)
ENDIF(OPENCL_FOUND)
