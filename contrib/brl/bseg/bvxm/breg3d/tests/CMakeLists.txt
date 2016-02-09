add_executable( breg3d_test_all
  test_driver.cxx
  test_ekf_camera_optimizer.cxx
# tests fail on dashboard FIX_ME
 # test_ekf_camera_optimizer_seq.cxx
 # test_homography_generator.cxx
 # test_ekf_existing_world.cxx
)

target_link_libraries( breg3d_test_all breg3d bvxm ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpgl bsta bsta_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl)

add_test( NAME breg3d_test_ekf_camera_optimizer COMMAND $<TARGET_FILE:breg3d_test_all> test_ekf_camera_optimizer)
#add_test( NAME breg3d_test_ekf_camera_optimizer_seq COMMAND $<TARGET_FILE:breg3d_test_all> test_ekf_camera_optimizer_seq)
#add_test( NAME breg3d_test_homography_generator COMMAND $<TARGET_FILE:breg3d_test_all> test_homography_generator)
#add_test( NAME breg3d_test_ekf_existing_world COMMAND $<TARGET_FILE:breg3d_test_all> test_ekf_existing_world)

add_executable( breg3d_test_include test_include.cxx )
target_link_libraries( breg3d_test_include breg3d)
