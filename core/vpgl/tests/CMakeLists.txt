add_executable( vpgl_test_all
  test_driver.cxx

  test_proj_camera.cxx
  test_perspective_camera.cxx
  test_perspective_camera_decomposition.cxx
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

target_link_libraries( vpgl_test_all ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vpgl_test_proj_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_proj_camera)
add_test( NAME vpgl_test_perspective_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_perspective_camera)
add_test( NAME test_perspective_camera_decomposition COMMAND $<TARGET_FILE:vpgl_test_all> test_perspective_camera_decomposition)
add_test( NAME vpgl_test_affine_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_affine_camera)
add_test( NAME vpgl_test_calibration_matrix COMMAND $<TARGET_FILE:vpgl_test_all> test_calibration_matrix)
add_test( NAME vpgl_test_poly_radial_distortion COMMAND $<TARGET_FILE:vpgl_test_all> test_poly_radial_distortion)
add_test( NAME vpgl_test_fundamental_matrix COMMAND $<TARGET_FILE:vpgl_test_all> test_fundamental_matrix)
add_test( NAME vpgl_test_essential_matrix COMMAND $<TARGET_FILE:vpgl_test_all> test_essential_matrix)
add_test( NAME vpgl_test_rational_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_rational_camera)
add_test( NAME vpgl_test_local_rational_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_local_rational_camera)
add_test( NAME vpgl_test_generic_camera COMMAND $<TARGET_FILE:vpgl_test_all> test_generic_camera)
add_test( NAME vpgl_test_lvcs COMMAND $<TARGET_FILE:vpgl_test_all> test_lvcs)
set( HAS_GEOTIFF 0 )
include( ${VXL_CMAKE_DIR}/FindGEOTIFF.cmake)
if(GEOTIFF_FOUND)
  set(HAS_GEOTIFF 1 )
  include_directories(${GEOTIFF_INCLUDE_DIR})
endif()

add_executable( vpgl_test_include test_include.cxx )
target_link_libraries( vpgl_test_include ${VXL_LIB_PREFIX}vpgl )
add_executable( vpgl_test_template_include test_template_include.cxx )
target_link_libraries( vpgl_test_template_include ${VXL_LIB_PREFIX}vpgl )
