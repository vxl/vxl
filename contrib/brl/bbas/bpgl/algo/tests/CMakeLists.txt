# temporary include
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

ADD_EXECUTABLE( bpgl_algo_test_all
  test_driver.cxx
  test_optimize_camera.cxx
  test_fm_compute.cxx
  test_lens_warp_mapper.cxx
  test_interpolate.cxx
  test_camera_homographies.cxx
  test_camera_from_box.cxx
  test_construct_cameras.cxx
  test_project.cxx
)

TARGET_LINK_LIBRARIES( bpgl_algo_test_all bpgl_algo vpgl_algo vgl_algo vgl vnl_algo vnl vil vul testlib )

CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/07JAN27.RPB ${CMAKE_CURRENT_BINARY_DIR}/07JAN27.RPB )

# the ransac implementations use rpl/rrel
IF(BUILD_RPL)
ADD_TEST( bpgl_algo_test_fm_compute                  ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_fm_compute )
ENDIF(BUILD_RPL)

ADD_TEST( bpgl_algo_test_construct_cameras           ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_construct_cameras )
ADD_TEST( bpgl_algo_test_optimize_camera             ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_optimize_camera )
ADD_TEST( bpgl_algo_test_interpolate                 ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_interpolate )
ADD_TEST( bpgl_algo_test_camera_homographies         ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_camera_homographies )
ADD_TEST( bpgl_algo_test_camera_from_box             ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_camera_from_box )
ADD_TEST( bpgl_algo_test_project                     ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_project )
ADD_TEST( bpgl_algo_test_lens_warp_mapper            ${EXECUTABLE_OUTPUT_PATH}/bpgl_algo_test_all test_lens_warp_mapper )

ADD_EXECUTABLE( bpgl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bpgl_algo_test_include bpgl_algo )
ADD_EXECUTABLE( bpgl_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bpgl_algo_test_template_include bpgl_algo )
