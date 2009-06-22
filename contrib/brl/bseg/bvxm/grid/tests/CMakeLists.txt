ADD_EXECUTABLE( bvxm_grid_test_all
  test_driver.cxx
  test_voxel_storage_mem.cxx
  test_voxel_storage_disk.cxx
  test_voxel_storage_disk_cached.cxx
  test_voxel_grid.cxx
  test_basic_ops.cxx
)

TARGET_LINK_LIBRARIES( bvxm_grid_test_all bvxm_grid testlib vcl vbl vpgl bsta bsta_algo vpl)

ADD_TEST( bvxm_test_voxel_storage_mem         ${EXECUTABLE_OUTPUT_PATH}/bvxm_grid_test_all   test_voxel_storage_mem )
ADD_TEST( bvxm_test_voxel_storage_disk        ${EXECUTABLE_OUTPUT_PATH}/bvxm_grid_test_all   test_voxel_storage_disk )
ADD_TEST( bvxm_test_voxel_storage_disk_cached ${EXECUTABLE_OUTPUT_PATH}/bvxm_grid_test_all   test_voxel_storage_disk_cached )
ADD_TEST( bvxm_test_voxel_grid                ${EXECUTABLE_OUTPUT_PATH}/bvxm_grid_test_all   test_voxel_grid )
ADD_TEST( bvxm_test_voxel_grid_basic_ops      ${EXECUTABLE_OUTPUT_PATH}/bvxm_grid_test_all   test_basic_ops )

ADD_EXECUTABLE( bvxm_grid_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bvxm_grid_test_include bvxm )
