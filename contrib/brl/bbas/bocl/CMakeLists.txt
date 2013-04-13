# basic open_cl library

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

  # some calls may be different for versions prior to 1.2 (e.g. clCreateImage2D)
  OPTION(OPENCL_IS_PRE_V1.2 "OpenCL library version prior to 1.2" ON)
  IF (OPENCL_IS_PRE_V1.2)
    ADD_DEFINITIONS(-DOPENCL_IS_PRE_V12)
  ENDIF(OPENCL_IS_PRE_V1.2)

  SET(bocl_sources
      bocl_manager.h      bocl_manager.txx  bocl_manager.cxx
      bocl_cl.h
      bocl_cl_gl.h
      bocl_device_info.h  bocl_device_info.cxx
      bocl_device.h       bocl_device.cxx
      bocl_utils.h        bocl_utils.cxx
      bocl_mem.h          bocl_mem.cxx
      bocl_kernel.h       bocl_kernel.cxx
  )
  AUX_SOURCE_DIRECTORY(Templates bocl_sources)
  ADD_LIBRARY(bocl ${bocl_sources} )

  TARGET_LINK_LIBRARIES(bocl brdb vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES})

  #install the .h .txx and libs
  INSTALL_TARGETS(/lib bocl)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bocl/ ${bocl_sources})

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#pro
SUBDIRS(pro)
ENDIF(OPENCL_FOUND)
