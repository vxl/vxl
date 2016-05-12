include( ${BRL_MODULE_PATH}/FindGLEW.cmake )

if(GLEW_FOUND)
 if(VGUI_FOUND)
  include_directories(${GLEW_INCLUDE_DIR} )
  link_directories(${OPENGL_LIBRARY_DIR})

  add_executable( bstm_ocl_render_view bstm_ocl_render_view.cxx )
  target_link_libraries( bstm_ocl_render_view boxm2 bstm_ocl_algo bstm_ocl_pro bstm_view ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

  add_executable( bstm_ocl_multi_render_view bstm_ocl_multi_render_view.cxx )
  target_link_libraries( bstm_ocl_multi_render_view boxm2 bstm_ocl_algo bstm_ocl_pro bstm_view ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

 endif()
endif()

