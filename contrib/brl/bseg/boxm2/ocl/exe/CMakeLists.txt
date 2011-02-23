# this is an executable to run a process for profiling purposes
ADD_EXECUTABLE( boxm2_render_expected_exe
  boxm2_render_expected_exe.cxx
)
TARGET_LINK_LIBRARIES( boxm2_render_expected_exe boxm2 boxm2_basic boxm2_io boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vul )

ADD_EXECUTABLE( boxm2_update_exe
  boxm2_update_exe.cxx
)
TARGET_LINK_LIBRARIES( boxm2_update_exe boxm2_ocl_pro vil_pro vpgl_pro vul )

ADD_EXECUTABLE( boxm2_update_rgb_exe
  boxm2_update_rgb_exe.cxx
)
TARGET_LINK_LIBRARIES( boxm2_update_rgb_exe boxm2_ocl_pro vil_pro vpgl_pro vul )

ADD_EXECUTABLE( boxm2_render_hemisphere
  boxm2_render_hemisphere.cxx
)
TARGET_LINK_LIBRARIES( boxm2_render_hemisphere boxm2 boxm2_basic boxm2_io boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph vul )

ADD_EXECUTABLE( boxm2_export_scene
  boxm2_export_scene.cxx
)
TARGET_LINK_LIBRARIES( boxm2_export_scene boxm2 boxm2_basic boxm2_io sdet boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph vul )


INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )

IF(GLEW_FOUND)
 IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE( boxm2_render_view boxm2_render_view.cxx)
  TARGET_LINK_LIBRARIES( boxm2_render_view boxm2 boxm2_view vul vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

  ADD_EXECUTABLE( boxm2_change_detection_view boxm2_change_detection_view.cxx)
  TARGET_LINK_LIBRARIES( boxm2_change_detection_view boxm2 boxm2_view vul vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

 ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)

