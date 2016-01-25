ADD_EXECUTABLE( bvxm_test_all
  test_driver.cxx
  test_apm_processors.cxx
  test_lidar_processor.cxx
  test_voxel_world.cxx
  test_voxel_world_update.cxx
  test_voxel_world_mog_image.cxx
  test_platform_computations.cxx
  test_tangent_update.cxx
  test_illum.cxx
)

TARGET_LINK_LIBRARIES( bvxm_test_all bvxm bvxm_grid testlib vpgl bsta bsta_algo vgl_algo vil vgl vnl vul vpl vbl vcl )

ADD_TEST( NAME bvxm_test_apm_processors COMMAND $<TARGET_FILE:bvxm_test_all>   test_apm_processors )
ADD_TEST( NAME bvxm_test_lidar_processor COMMAND $<TARGET_FILE:bvxm_test_all>   test_lidar_processor )
ADD_TEST( NAME bvxm_test_voxel_world COMMAND $<TARGET_FILE:bvxm_test_all>   test_voxel_world )
ADD_TEST( NAME bvxm_test_voxel_world_update COMMAND $<TARGET_FILE:bvxm_test_all>   test_voxel_world_update )
ADD_TEST( NAME bvxm_test_voxel_world_mog_image COMMAND $<TARGET_FILE:bvxm_test_all>   test_voxel_world_mog_image )
ADD_TEST( NAME bvxm_test_platform_computations COMMAND $<TARGET_FILE:bvxm_test_all>   test_platform_computations )
ADD_TEST( NAME bvxm_test_tangent_update COMMAND $<TARGET_FILE:bvxm_test_all>   test_tangent_update )
ADD_TEST( NAME bvxm_test_illum COMMAND $<TARGET_FILE:bvxm_test_all>   test_illum )

ADD_EXECUTABLE( bvxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_test_include bvxm bvxm_io bvxm_grid )
ADD_EXECUTABLE( bvxm_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_test_template_include bvxm bvxm_io bvxm_grid )
