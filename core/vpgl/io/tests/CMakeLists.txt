add_executable( vpgl_io_test_all
  test_driver.cxx
  test_lvcs_io.cxx
  test_camera_io.cxx
  test_proj_camera_io.cxx
  test_perspective_camera_io.cxx
  test_affine_camera_io.cxx
  test_rational_camera_io.cxx
  test_local_rational_camera_io.cxx
)
target_link_libraries( vpgl_io_test_all vpgl vpgl_io vgl_algo vgl vgl_algo vnl vbl vpl testlib )
add_test( NAME vpgl_test_lvcs_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_lvcs_io)
add_test( NAME vpgl_test_camera_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_camera_io)
add_test( NAME vpgl_test_proj_camera_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_proj_camera_io)
add_test( NAME vpgl_test_perspective_camera_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_perspective_camera_io)
add_test( NAME vpgl_test_affine_camera_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_affine_camera_io)
add_test( NAME vpgl_test_rational_camera_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_rational_camera_io)
add_test( NAME vpgl_test_local_rational_camera_io COMMAND $<TARGET_FILE:vpgl_io_test_all> test_local_rational_camera_io)

add_executable( vpgl_io_test_include test_include.cxx )
target_link_libraries( vpgl_io_test_include vpgl_io)
add_executable( vpgl_io_test_template_include test_template_include.cxx )
target_link_libraries( vpgl_io_test_template_include vpgl_io)
