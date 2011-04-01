INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  # executable to convert a bit scene to a boxm2_scene
  ADD_EXECUTABLE( boxm2_to_boxm_exe boxm2_to_boxm.cxx  boxm2_to_boxm.h)

  ADD_EXECUTABLE( boxm_to_boxm2_exe boxm_to_boxm2.cxx  boxm_to_boxm2.h)

  TARGET_LINK_LIBRARIES( boxm2_to_boxm_exe boxm2 boxm boxm_sample boct vbl vnl vcl)
  TARGET_LINK_LIBRARIES( boxm_to_boxm2_exe boxm2 boxm boxm_sample boct vbl vnl vcl)

  #tests
  SUBDIRS(tests)

ENDIF(OPENCL_FOUND)
