ADD_EXECUTABLE( breg3d_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
  test_ekf_camera_optimizer_seq.cxx
  test_homography_generator.cxx
)

TARGET_LINK_LIBRARIES( breg3d_test_all breg3d bvxm testlib vcl vbl vgl vpgl bsta bsta_algo)


ADD_TEST( breg3d_test_ekf_camera_optimizer     ${EXECUTABLE_OUTPUT_PATH}/breg3d_test_all   test_ekf_camera_optimizer)
ADD_TEST( breg3d_test_ekf_camera_optimizer_seq ${EXECUTABLE_OUTPUT_PATH}/breg3d_test_all   test ekf_camera_optimizer_seq)
ADD_TEST( breg3d_test_homography_generator     ${EXECUTABLE_OUTPUT_PATH}/breg3d_test_all   test_homography_generator)





