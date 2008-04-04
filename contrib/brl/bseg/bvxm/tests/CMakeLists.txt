ADD_EXECUTABLE( bvxm_test_all
  test_driver.cxx
  test_voxel_storage_mem.cxx
  test_voxel_storage_disk.cxx
  test_voxel_grid.cxx
  test_apm_processors.cxx
  test_voxel_world.cxx
  test_voxel_world_update.cxx
  test_voxel_world_mog_image.cxx
  test_gen_synthetic_world.cxx
  test_lidar_camera.cxx
)

TARGET_LINK_LIBRARIES( bvxm_test_all bvxm testlib vcl vbl vpgl bsta bsta_algo)

ADD_TEST( bvxm_test_voxel_storage_mem    ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_storage_mem )
ADD_TEST( bvxm_test_voxel_storage_disk   ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_storage_disk )
ADD_TEST( bvxm_test_voxel_grid           ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_grid )
ADD_TEST( bvxm_test_apm_processors   ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_apm_processors )
ADD_TEST( bvxm_test_voxel_world      ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_world )
ADD_TEST( bvxm_test_voxel_world_update      ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_world_update )
ADD_TEST( bvxm_test_voxel_world_mog_image   ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_voxel_world_mog_image )
ADD_TEST( bvxm_test_gen_synthetic_world     ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_gen_synthetic_world )
ADD_TEST( bvxm_test_lidar_camera     ${EXECUTABLE_OUTPUT_PATH}/bvxm_test_all   test_lidar_camera )

ADD_EXECUTABLE( bvxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_test_include bvxm )
