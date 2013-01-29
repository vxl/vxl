INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )

IF(GLEW_FOUND)
 IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE( bstm_ocl_render_view bstm_ocl_render_view.cxx )
  TARGET_LINK_LIBRARIES( bstm_ocl_render_view boxm2 bstm_ocl_algo bstm_ocl_pro bstm_view vul vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

  ADD_EXECUTABLE( bstm_ocl_multi_render_view bstm_ocl_multi_render_view.cxx )
  TARGET_LINK_LIBRARIES( bstm_ocl_multi_render_view boxm2 bstm_ocl_algo bstm_ocl_pro bstm_view vul vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )
  
 ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)

