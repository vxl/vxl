ADD_EXECUTABLE( boxm2_view_test_all
  test_driver.cxx
  test_camera_to_vrml.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_view_test_all testlib boxm2_view vpgl )

ADD_TEST( NAME boxm2_view_test_camera_to_vrml COMMAND $<TARGET_FILE:boxm2_view_test_all>  test_camera_to_vrml  )

ADD_EXECUTABLE( boxm2_view_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_view_test_include boxm2_view )
