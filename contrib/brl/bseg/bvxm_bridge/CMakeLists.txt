INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)

  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

  #find expatpp library
  INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  IF(EXPAT_FOUND)
    INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

    # executable to convert a bvxm scene to a boxm2_scene
    ADD_EXECUTABLE( bvxm_to_boxm2_exe bvxm_to_boxm2.cxx  bvxm_to_boxm2.h)
    TARGET_LINK_LIBRARIES( bvxm_to_boxm2_exe boxm2 bvxm bvxm_io bvxm_grid_io bvxm_algo boxm2_io boct vnl vgl vul vbl vcl)

  ENDIF(EXPAT_FOUND)


ENDIF(OPENCL_FOUND)
