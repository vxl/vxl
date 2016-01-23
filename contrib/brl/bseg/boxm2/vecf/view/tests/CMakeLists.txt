# no testlib tests for the moment, just the rendering executable
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIR} )
  link_directories(${OPENGL_LIBRARY_DIR})

  add_executable(boxm2_vecf_articulated_render articulated_render.cxx )
  target_link_libraries(boxm2_vecf_articulated_render
    bgui boxm2_vecf_view boxm2_vecf_ocl boxm2_view brdb
    vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl vgui ${GLEW_LIBRARY})
endif()

add_executable(boxm2_vecf_display_orbit orbit_viewer.cxx)
target_link_libraries(boxm2_vecf_display_orbit
  boxm2_vecf_view boxm2_vecf boxm2_view bgui brdb
  vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl vgui)
