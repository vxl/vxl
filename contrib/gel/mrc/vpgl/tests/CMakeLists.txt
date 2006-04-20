ADD_EXECUTABLE( vpgl_test_all
  test_driver.cxx

  test_proj_camera.cxx
  test_perspective_camera.cxx
  test_affine_camera.cxx
  test_fundamental_matrix.cxx
  test_calibration_matrix.cxx
  test_camera_io.cxx
  test_poly_radial_distortion.cxx
)

TARGET_LINK_LIBRARIES( vpgl_test_all vpgl vpl testlib )

ADD_TEST( vpgl_test_proj_camera                vpgl_test_all test_proj_camera )
ADD_TEST( vpgl_test_perspective_camera         vpgl_test_all test_perspective_camera)
ADD_TEST( vpgl_test_affine_camera              vpgl_test_all test_affine_camera)
ADD_TEST( vpgl_test_calibration_matrix         vpgl_test_all test_calibration_matrix)
ADD_TEST( vpgl_test_camera_io                  vpgl_test_all test_camera_io)
ADD_TEST( vpgl_test_poly_radial_distortion     vpgl_test_all test_poly_radial_distortion)
ADD_TEST( vpgl_test_fundamental_matrix         vpgl_test_all test_fundamental_matrix)

ADD_EXECUTABLE( vpgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_test_include vpgl )
