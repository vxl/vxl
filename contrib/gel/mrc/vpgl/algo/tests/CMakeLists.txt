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
  test_ortho_procrustes.cxx
  test_bundle_adjust.cxx
)

TARGET_LINK_LIBRARIES( vpgl_algo_test_all vpgl_algo testlib mvl vil )

ADD_TEST( vpgl_algo_test_construct_cameras  ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_construct_cameras )
ADD_TEST( vpgl_algo_test_project            ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_project )
ADD_TEST( vpgl_algo_test_optimize_camera    ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_optimize_camera )
ADD_TEST( vpgl_algo_test_fm_compute         ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_fm_compute )
ADD_TEST( vpgl_algo_test_lens_warp_mapper   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_lens_warp_mapper )
ADD_TEST( vpgl_algo_test_camera_compute     ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_camera_compute ${CMAKE_CURRENT_SOURCE_DIR})
ADD_TEST( vpgl_algo_test_backproject        ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_backproject )
ADD_TEST( vpgl_algo_test_ortho_procrustes   ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_ortho_procrustes )
ADD_TEST( vpgl_algo_test_bundle_adjust      ${EXECUTABLE_OUTPUT_PATH}/vpgl_algo_test_all test_bundle_adjust )

ADD_EXECUTABLE( vpgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vpgl_algo_test_include vpgl_algo )
