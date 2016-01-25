ADD_EXECUTABLE( bvxm_grid_test_all
  test_driver.cxx
  test_voxel_storage_mem.cxx
  test_voxel_storage_slab_mem.cxx
  test_voxel_storage_disk.cxx
  test_voxel_storage_disk_cached.cxx
  test_voxel_grid.cxx
  test_basic_ops.cxx
  test_grid_to_image_stack.cxx
  test_bvxm_vrml.cxx
)

TARGET_LINK_LIBRARIES( bvxm_grid_test_all bvxm_grid bvxm_grid_io testlib vpgl bsta bsta_algo vnl vil vgl vul vpl vbl vcl )

ADD_TEST( NAME bvxm_grid_test_voxel_storage_mem COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_mem )
ADD_TEST( NAME bvxm_grid_test_voxel_storage_slab_mem COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_slab_mem )
ADD_TEST( NAME bvxm_grid_test_voxel_storage_disk COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_disk )
ADD_TEST( NAME bvxm_grid_test_voxel_storage_disk_cached COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_disk_cached )
ADD_TEST( NAME bvxm_grid_test_voxel_grid COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_grid )
ADD_TEST( NAME bvxm_grid_test_basic_ops COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_basic_ops )
ADD_TEST( NAME bvxm_grid_test_grid_to_image_stack COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_grid_to_image_stack )
ADD_TEST( NAME bvxm_grid_test_vrml COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_bvxm_vrml )

ADD_EXECUTABLE( bvxm_grid_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_grid_test_include bvxm )
ADD_EXECUTABLE( bvxm_grid_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_grid_test_template_include bvxm )
