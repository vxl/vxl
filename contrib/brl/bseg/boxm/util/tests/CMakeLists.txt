add_executable( boxm_util_test_all
  test_driver.cxx
  test_boxm_plane_ransac.cxx
  test_boxm_rational_camera_utils.cxx
  test_quad_interpolate.cxx
 )

target_link_libraries( boxm_util_test_all boxm_util boxm boxm_sample ${VXL_LIB_PREFIX}testlib boct bsta ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vil bvrml)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

add_test( NAME boxm_util_test_plane_ransac COMMAND $<TARGET_FILE:boxm_util_test_all> test_boxm_plane_ransac  )
add_test( NAME boxm_util_test_rational_camera_utils COMMAND $<TARGET_FILE:boxm_util_test_all> test_boxm_rational_camera_utils  ${CMAKE_CURRENT_SOURCE_DIR})
add_test( NAME boxm_util_test_quad_interpolate COMMAND $<TARGET_FILE:boxm_util_test_all> test_quad_interpolate  ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

add_executable( boxm_util_test_include test_include.cxx )
target_link_libraries( boxm_util_test_include boxm_util)
add_executable( boxm_util_test_template_include test_template_include.cxx )
target_link_libraries( boxm_util_test_template_include boxm_util)
