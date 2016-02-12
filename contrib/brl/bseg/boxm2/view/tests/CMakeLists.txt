add_executable( boxm2_view_test_all
  test_driver.cxx
  test_camera_to_vrml.cxx
 )

target_link_libraries( boxm2_view_test_all ${VXL_LIB_PREFIX}testlib boxm2_view ${VXL_LIB_PREFIX}vpgl )

add_test( NAME boxm2_view_test_camera_to_vrml COMMAND $<TARGET_FILE:boxm2_view_test_all>  test_camera_to_vrml  )

add_executable( boxm2_view_test_include test_include.cxx )
target_link_libraries( boxm2_view_test_include boxm2_view )
