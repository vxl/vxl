
ADD_EXECUTABLE( boxm2_render_hemisphere boxm2_render_hemisphere.cxx )
TARGET_LINK_LIBRARIES( boxm2_render_hemisphere boxm2 boxm2_basic boxm2_io boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph vil vbl vul)

ADD_EXECUTABLE( boxm2_export_scene boxm2_export_scene.cxx )
TARGET_LINK_LIBRARIES( boxm2_export_scene boxm2 boxm2_basic boxm2_io sdet boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph vil vbl vul)

ADD_EXECUTABLE( boxm2_ocl_refine_scene boxm2_ocl_refine_scene.cxx )
TARGET_LINK_LIBRARIES(  boxm2_ocl_refine_scene boxm2 boxm2_pro boxm2_basic boxm2_io boxm2_ocl boxm2_ocl_pro bocl vil_pro)

ADD_EXECUTABLE(boxm2_estimate_uncertain_viewpoints boxm2_estimate_uncertain_viewpoints.cxx )
TARGET_LINK_LIBRARIES( boxm2_estimate_uncertain_viewpoints boxm2 boxm2_basic boxm2_io sdet boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph vil vbl vul)

INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )

IF(GLEW_FOUND)
 IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE( boxm2_ocl_render_view boxm2_ocl_render_view.cxx )
  TARGET_LINK_LIBRARIES( boxm2_ocl_render_view boxm2 boxm2_view vul vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

 ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)

