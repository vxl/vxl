ADD_EXECUTABLE( boxm2_vecf_view_test_all
  test_driver.cxx
#  test_articulated_render.cxx
 )

INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )

IF(GLEW_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE(boxm2_test_articulated_render test_articulated_render.cxx )	
TARGET_LINK_LIBRARIES(boxm2_test_articulated_render boxm2_vecf_view boxm2_vecf boxm2_view brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl vgui ${GLEW_LIBRARY})

#ADD_TEST( boxm2_articulated_render      ${EXECUTABLE_OUTPUT_PATH}/boxm2_vecf_view_test_all  test_articulated_render  )
ENDIF(GLEW_FOUND)
#ADD_EXECUTABLE( boxm2_vecf_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_vecf_test_include boxm2_vecf )

