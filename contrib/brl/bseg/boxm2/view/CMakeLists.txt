# boxm opencl opengl display
  INCLUDE( ${VXL_CMAKE_DIR}/NewCMake/FindOpenCL.cmake )
  IF(OPENCL_FOUND)
  
    ADD_DEFINITIONS(-DHAS_OPENCL=1)
    INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )
IF(GLEW_FOUND)
IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  SET(boxm2_view_sources
      boxm2_cam_tableau.h
      boxm2_cam_tableau.cxx
      boxm2_render_tableau.h
      boxm2_render_tableau.cxx
      boxm2_change_detection_tableau.h
      boxm2_change_detection_tableau.cxx
      boxm2_include_glew.h
     )

  ADD_LIBRARY(boxm2_view ${boxm2_view_sources} )

  TARGET_LINK_LIBRARIES(boxm2_view bocl   boxm2_ocl_pro boxm2_cpp  bprb bgeo vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${GLEW_LIBRARY} ${OPENCL_LIBRARIES} )

ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)
ENDIF(OPENCL_FOUND)