ADD_EXECUTABLE( boxm_util_test_all
  test_driver.cxx
  test_boxm_plane_ransac.cxx
  test_boxm_rational_camera_utils.cxx
  test_quad_interpolate.cxx
 )

TARGET_LINK_LIBRARIES( boxm_util_test_all boxm_util boxm boxm_sample testlib boct bsta vcl vnl vnl_algo vnl_io vgl vgl_algo vbl vpgl vpgl_algo vil bvrml)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

ADD_TEST( boxm_util_test_plane_ransac          ${EXECUTABLE_OUTPUT_PATH}/boxm_util_test_all test_boxm_plane_ransac  )
ADD_TEST( boxm_util_test_rational_camera_utils ${EXECUTABLE_OUTPUT_PATH}/boxm_util_test_all test_boxm_rational_camera_utils  ${CMAKE_CURRENT_SOURCE_DIR})
ADD_TEST( boxm_util_test_quad_interpolate      ${EXECUTABLE_OUTPUT_PATH}/boxm_util_test_all test_quad_interpolate  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

ADD_EXECUTABLE( boxm_util_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_util_test_include boxm_util)
ADD_EXECUTABLE( boxm_util_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( boxm_util_test_template_include boxm_util)
