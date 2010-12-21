# this is an executable to run a process for profiling purposes
ADD_EXECUTABLE( boxm2_render_expected_exe
  boxm2_render_expected_exe.cxx
)
TARGET_LINK_LIBRARIES( boxm2_render_expected_exe boxm2 boxm2_io bocl vil_pro vpgl_pro vul )

ADD_EXECUTABLE( boxm2_convert_bit_scene_exe
  boxm2_convert_bit_scene.cxx
)
TARGET_LINK_LIBRARIES( boxm2_convert_bit_scene_exe boxm2 boxm2_io boxm_ocl vbl vnl vcl)

ADD_EXECUTABLE( boxm2_rep_scene
  boxm2_rep_scene.cxx
)
TARGET_LINK_LIBRARIES( boxm2_rep_scene boxm2 boxm2_io boxm_ocl vbl vnl vcl)

INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )

IF(GLEW_FOUND)
 IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE( boxm2_render_view boxm2_render_view.cxx)
  TARGET_LINK_LIBRARIES( boxm2_render_view boxm2 boxm2_view vul vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

  #ADD_EXECUTABLE( boxm_ocl_update_view boxm_ocl_update_view.cxx)
  #TARGET_LINK_LIBRARIES( boxm_ocl_update_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  #ADD_EXECUTABLE( boxm_opencl_change_detection_view boxm_opencl_change_detection_view.cxx)
  #TARGET_LINK_LIBRARIES( boxm_opencl_change_detection_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  #ADD_EXECUTABLE( boxm_ocl_rerender_view boxm_ocl_rerender_view.cxx)
  #TARGET_LINK_LIBRARIES( boxm_ocl_rerender_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  #ADD_EXECUTABLE( boxm_ocl_render_bit_view boxm_ocl_render_bit_view.cxx)
  #TARGET_LINK_LIBRARIES( boxm_ocl_render_bit_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  #ADD_EXECUTABLE( boxm_ocl_update_bit_view boxm_ocl_update_bit_view.cxx)
  #TARGET_LINK_LIBRARIES( boxm_ocl_update_bit_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  #ADD_EXECUTABLE( boxm_ocl_change_detection_bit_view boxm_ocl_change_detection_bit_view.cxx)
  #TARGET_LINK_LIBRARIES( boxm_ocl_change_detection_bit_view boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

 ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)

