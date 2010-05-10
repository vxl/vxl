ADD_EXECUTABLE( boxm2_util_test_all
  test_driver.cxx
  test_boxm_plane_ransac.cxx
  test_boxm_rational_camera_utils.cxx
  test_quad_interpolate.cxx
  test_sigma_normalizer.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_util_test_all boxm2_util boxm2 boxm2_sample testlib boct bsta vcl vnl vnl_algo vnl_io vgl vgl_algo vbl vpgl vpgl_algo vil bvrml)

ADD_TEST( boxm2_test_boxm_plane_ransac    ${EXECUTABLE_OUTPUT_PATH}/boxm2_util_test_all test_boxm_plane_ransac  )
ADD_TEST( boxm2_test_boxm_rational_camera_utils   ${EXECUTABLE_OUTPUT_PATH}/boxm2_util_test_all test_boxm_rational_camera_utils  )
ADD_TEST( boxm2_test_quad_interpolate   ${EXECUTABLE_OUTPUT_PATH}/boxm2_util_test_all test_quad_interpolate  )
ADD_TEST( boxm2_test_sigma_normalizer   ${EXECUTABLE_OUTPUT_PATH}/boxm2_util_test_all test_sigma_normalizer  )

ADD_EXECUTABLE( boxm2_util_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_util_test_include boxm2_util)
