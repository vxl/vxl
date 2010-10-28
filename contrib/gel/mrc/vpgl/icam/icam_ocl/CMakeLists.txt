# icam open_cl library

INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl/icam )

  SET(icam_ocl_sources
      icam_ocl_cl.h
      icam_ocl_cl_gl.h
      icam_ocl_utils.h          icam_ocl_utils.cxx
      icam_ocl_manager.h        icam_ocl_manager.txx
      icam_ocl_search_manager.h icam_ocl_search_manager.cxx
      icam_ocl_mem.h            icam_ocl_mem.cxx
      icam_ocl_kernel.h         icam_ocl_kernel.cxx
  )
  AUX_SOURCE_DIRECTORY(Templates icam_ocl_sources)
  ADD_LIBRARY(icam_ocl ${icam_ocl_sources} )

  TARGET_LINK_LIBRARIES(icam_ocl vpgl vpgl_algo icam  vgl vil vbl vul ${OPENCL_LIBRARIES})

#tests
SUBDIRS(tests)

ENDIF(OPENCL_FOUND)
