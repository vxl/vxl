# boxm opencl opengl display
INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
IF(OPENCL_FOUND)
  ADD_DEFINITIONS(-DHAS_OPENCL=1)
  INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
  INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )
IF(GLEW_FOUND)
  ADD_DEFINITIONS(-DHAS_GLEW=1)

IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  SET(bstm_view_sources
      bstm_include_glew.h
      bstm_cam_tableau.h        bstm_cam_tableau.cxx
      bstm_ocl_render_tableau.h  bstm_ocl_render_tableau.cxx
      bstm_ocl_multi_render_tableau.h  bstm_ocl_multi_render_tableau.cxx
     )
  ADD_LIBRARY(bstm_view ${bstm_view_sources} )

  TARGET_LINK_LIBRARIES(bstm_view bocl boxm2_view bstm_ocl_pro vgui bprb vnl vnl_algo vbl_io vpgl vsph vgl vil vbl vul ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} )

  IF( BUILD_TESTING )
    SUBDIRS(tests)
  ENDIF( BUILD_TESTING )

ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)
ENDIF(OPENCL_FOUND)
