# temporary include
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

ADD_EXECUTABLE( vpgl_algo_test_all
  test_driver.cxx
  test_construct_cameras.cxx
  test_project.cxx
  test_optimize_camera.cxx
  test_fm_compute.cxx
  test_camera_compute.cxx
  test_lens_warp_mapper.cxx
  test_backproject.cxx
  test_ray.cxx
  test_ortho_procrustes.cxx
  test_ray_intersect.cxx
  test_adjust_rational_trans_onept.cxx
  test_rational_geo_adjust.cxx
  test_bundle_adjust.cxx
  test_interpolate.cxx
  test_camera_homographies.cxx
)

TARGET_LINK_LIBRARIES( vpgl_algo_test_all vpgl_algo testlib mvl vil )

CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/07JAN27.RPB ${CMAKE_CURRENT_BINARY_DIR}/07JAN27.RPB )

ADD_TEST( vpgl_algo_test_camera_compute     ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_camera_compute )
ADD_TEST( vpgl_algo_test_construct_cameras  ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_construct_cameras )
ADD_TEST( vpgl_algo_test_project            ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_project )
ADD_TEST( vpgl_algo_test_optimize_camera    ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_optimize_camera )
ADD_TEST( vpgl_algo_test_fm_compute         ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_fm_compute )
ADD_TEST( vpgl_algo_test_lens_warp_mapper   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_lens_warp_mapper )
ADD_TEST( vpgl_algo_test_ray_intersect     ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ray_intersect )
ADD_TEST( vpgl_algo_test_adjust_rational_trans_onept   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_adjust_rational_trans_onept )
ADD_TEST( vpgl_algo_test_rational_geo_adjust   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_rational_geo_adjust )
ADD_TEST( vpgl_algo_test_backproject        ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_backproject )
ADD_TEST( vpgl_algo_test_ray      ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ray )
ADD_TEST( vpgl_algo_test_ortho_procrustes   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ortho_procrustes )
ADD_TEST( vpgl_algo_test_bundle_adjust      ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_bundle_adjust )
ADD_TEST( vpgl_algo_test_interpolate      ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_interpolate )
ADD_TEST( vpgl_algo_test_camera_homographies      ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_camera_homographies )

ADD_EXECUTABLE( vpgl_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( vpgl_algo_test_include vpgl_algo )

