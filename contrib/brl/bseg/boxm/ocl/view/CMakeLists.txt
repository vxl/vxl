# boxm opencl opengl display
INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )
IF(GLEW_FOUND)
IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
  INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
  INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  SET(boxm_ocl_view_sources
      boxm_ocl_draw_glbuffer_tableau.h 
      boxm_ocl_rerender_tableau.h 
      boxm_ocl_rerender_tableau_sptr.h
      boxm_ocl_rerender_tableau.cxx 
      boxm_ocl_draw_glbuffer_tableau.cxx
      boxm_ocl_draw_glbuffer_tableau_sptr.h
      boxm_ocl_update_tableau.h 
      boxm_ocl_update_tableau.cxx
      boxm_ocl_update_tableau_sptr.h
      boxm_ocl_change_detection_tableau.h 
      boxm_ocl_change_detection_tableau.cxx
      boxm_ocl_change_detection_tableau_sptr.h
      boxm_cam_tableau.h
      boxm_cam_tableau.cxx
      boxm_cam_tableau_sptr.h
     )

  
  ADD_LIBRARY(boxm_ocl_view ${boxm_ocl_view_sources} )

  TARGET_LINK_LIBRARIES(boxm_ocl_view bocl boxm boxm_ocl boxm_util boxm_sample boxm_basic boct bprb bgeo vnl vnl_algo vbl_io vpgl vgl vil vbl vul ${OPENCL_LIBRARIES} ${GLEW_LIBRARY})


ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)
