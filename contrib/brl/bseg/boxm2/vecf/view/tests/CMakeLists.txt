# no testlib tests for the moment, just the rendering executable
IF(GLEW_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE(boxm2_vecf_articulated_render articulated_render.cxx )
TARGET_LINK_LIBRARIES(boxm2_vecf_articulated_render boxm2_vecf_view boxm2_vecf_ocl boxm2_view brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl vgui ${GLEW_LIBRARY})
ENDIF(GLEW_FOUND)

  ADD_EXECUTABLE(boxm2_vecf_display_orbit orbit_viewer.cxx)
TARGET_LINK_LIBRARIES(boxm2_vecf_display_orbit boxm2_vecf_view boxm2_vecf boxm2_view bgui brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl vgui)
