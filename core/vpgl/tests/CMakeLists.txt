ADD_EXECUTABLE( vpgl_test_all
  test_driver.cxx

  test_proj_camera.cxx
  test_perspective_camera.cxx
  test_affine_camera.cxx
  test_fundamental_matrix.cxx
  test_essential_matrix.cxx
  test_calibration_matrix.cxx
  test_poly_radial_distortion.cxx
  test_rational_camera.cxx
  test_local_rational_camera.cxx
  test_generic_camera.cxx
  test_lvcs.cxx
)

TARGET_LINK_LIBRARIES( vpgl_test_all vpgl vpgl_io vgl_algo vgl vnl vbl vpl testlib )

ADD_TEST( NAME vpgl_test_proj_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_proj_camera)
ADD_TEST( NAME vpgl_test_perspective_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_perspective_camera)
ADD_TEST( NAME vpgl_test_affine_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_affine_camera)
ADD_TEST( NAME vpgl_test_calibration_matrix COMMAND $<TARGET_FILE:vpgl_test_all> test_calibration_matrix)
ADD_TEST( NAME vpgl_test_poly_radial_distortion COMMAND $<TARGET_FILE:vpgl_test_all> test_poly_radial_distortion)
ADD_TEST( NAME vpgl_test_fundamental_matrix COMMAND $<TARGET_FILE:vpgl_test_all> test_fundamental_matrix)
ADD_TEST( NAME vpgl_test_essential_matrix COMMAND $<TARGET_FILE:vpgl_test_all> test_essential_matrix)
ADD_TEST( NAME vpgl_test_rational_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_rational_camera)
ADD_TEST( NAME vpgl_test_local_rational_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_local_rational_camera)
ADD_TEST( NAME vpgl_test_generic_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_generic_camera)
ADD_TEST( NAME vpgl_test_lvcs COMMAND $<TARGET_FILE:vpgl_test_all> test_lvcs)
SET( HAS_GEOTIFF 0 )
INCLUDE( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
IF(GEOTIFF_FOUND)
  SET (HAS_GEOTIFF 1 )
  INCLUDE_DIRECTORIES(${GEOTIFF_INCLUDE_DIR})
ENDIF(GEOTIFF_FOUND)

ADD_EXECUTABLE( vpgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_test_include vpgl )
ADD_EXECUTABLE( vpgl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_test_template_include vpgl )
