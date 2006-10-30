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
)

TARGET_LINK_LIBRARIES( vpgl_algo_test_all vpgl_algo testlib mvl vil )

ADD_TEST( vpgl_algo_test_construct_cameras  ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_construct_cameras )
ADD_TEST( vpgl_algo_test_project            ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_project )
ADD_TEST( vpgl_algo_test_optimize_camera    ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_optimize_camera )
ADD_TEST( vpgl_algo_test_fm_compute         ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_fm_compute )
ADD_TEST( vpgl_algo_test_lens_warp_mapper   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_lens_warp_mapper )
ADD_TEST( vpgl_algo_test_camera_compute     ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_camera_compute )
ADD_TEST( vpgl_algo_test_backproject     ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_backproject )
ADD_EXECUTABLE( vpgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_algo_test_include vpgl_algo )
