INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} ) 

  ADD_EXECUTABLE( boxm2_to_boxm_exe boxm2_to_boxm.cxx  boxm2_to_boxm.h)
  TARGET_LINK_LIBRARIES( boxm2_to_boxm_exe boxm2 boxm boxm_sample boxm2_io boct vnl vgl vul vbl vcl)

  # executable to convert a bit scene to a boxm2_scene
  ADD_EXECUTABLE( boxm_to_boxm2_exe boxm_to_boxm2.cxx  boxm_to_boxm2.h)
  TARGET_LINK_LIBRARIES( boxm_to_boxm2_exe boxm2 boxm boxm_sample boxm2_io boct vnl vgl vul vbl vcl)

  #tests
  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(OPENCL_FOUND)
