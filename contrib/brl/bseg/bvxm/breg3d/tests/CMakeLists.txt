ADD_EXECUTABLE( breg3d_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
# tests fail on dashboard FIX_ME
 # test_ekf_camera_optimizer_seq.cxx
 # test_homography_generator.cxx
 # test_ekf_existing_world.cxx
)

TARGET_LINK_LIBRARIES( breg3d_test_all breg3d bvxm testlib vpgl bsta bsta_algo vgl_algo vil vgl vnl vbl vul vcl)

ADD_TEST( NAME breg3d_test_ekf_camera_optimizer COMMAND $<TARGET_FILE:breg3d_test_all> test_ekf_camera_optimizer)
#ADD_TEST( NAME breg3d_test_ekf_camera_optimizer_seq COMMAND $<TARGET_FILE:breg3d_test_all> test_ekf_camera_optimizer_seq)
#ADD_TEST( NAME breg3d_test_homography_generator COMMAND $<TARGET_FILE:breg3d_test_all> test_homography_generator)
#ADD_TEST( NAME breg3d_test_ekf_existing_world COMMAND $<TARGET_FILE:breg3d_test_all> test_ekf_existing_world)

ADD_EXECUTABLE( breg3d_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( breg3d_test_include breg3d)
