INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

  # executable to convert a bit scene to a boxm2_scene
  ADD_EXECUTABLE( boxm2_to_boxm_exe
    boxm2_to_boxm.cxx
  )
  TARGET_LINK_LIBRARIES( boxm2_to_boxm_exe boxm2 boxm boxm_sample vbl vnl vcl)

ENDIF(OPENCL_FOUND)
