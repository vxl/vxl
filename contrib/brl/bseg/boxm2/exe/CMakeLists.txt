# this is an executable to run a process for profiling purposes
ADD_EXECUTABLE( boxm2_render_expected_exe
  boxm2_render_expected_exe.cxx
)
TARGET_LINK_LIBRARIES( boxm2_render_expected_exe boxm2 boxm2_basic boxm2_io sdet boxm2_ocl bocl vil_pro vpgl_pro vul )

#executable to run a single update
ADD_EXECUTABLE( boxm2_update_exe
  boxm2_update_exe.cxx
)
TARGET_LINK_LIBRARIES( boxm2_update_exe boxm2 boxm2_basic boxm2_io sdet boxm2_ocl bocl vil_pro vpgl_pro vul )

# executable to convert a bit scene to a boxm2_scene
ADD_EXECUTABLE( boxm2_convert_bit_scene_exe
  boxm2_convert_bit_scene.cxx
)
TARGET_LINK_LIBRARIES( boxm2_convert_bit_scene_exe boxm2 boxm2_io boxm_ocl vbl vnl vcl)

#executable to replicate a scene (four times for tests)
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


 ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)

