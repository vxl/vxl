ADD_EXECUTABLE( bvxm_test_all
  test_driver.cxx
  test_apm_processors.cxx
  test_lidar_processor.cxx
  test_voxel_world.cxx
  test_voxel_world_update.cxx
  test_voxel_world_mog_image.cxx
  test_platform_computations.cxx
)

TARGET_LINK_LIBRARIES( bvxm_test_all bvxm bvxm_grid testlib vcl vbl vpgl bsta bsta_algo vpl)

ADD_TEST( bvxm_test_apm_processors            ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_apm_processors )
ADD_TEST( bvxm_test_lidar_processor           ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_lidar_processor )
ADD_TEST( bvxm_test_voxel_world               ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_world )
ADD_TEST( bvxm_test_voxel_world_update        ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_world_update )
ADD_TEST( bvxm_test_voxel_world_mog_image     ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_world_mog_image )
ADD_TEST( bvxm_test_platform_computations     ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_platform_computations )

ADD_EXECUTABLE( bvxm_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bvxm_test_include bvxm )
