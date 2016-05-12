add_executable( bvxm_grid_test_all
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

target_link_libraries( bvxm_grid_test_all bvxm_grid bvxm_grid_io ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpgl bsta bsta_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl )

add_test( NAME bvxm_grid_test_voxel_storage_mem COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_mem )
add_test( NAME bvxm_grid_test_voxel_storage_slab_mem COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_slab_mem )
add_test( NAME bvxm_grid_test_voxel_storage_disk COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_disk )
add_test( NAME bvxm_grid_test_voxel_storage_disk_cached COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_storage_disk_cached )
add_test( NAME bvxm_grid_test_voxel_grid COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_voxel_grid )
add_test( NAME bvxm_grid_test_basic_ops COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_basic_ops )
add_test( NAME bvxm_grid_test_grid_to_image_stack COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_grid_to_image_stack )
add_test( NAME bvxm_grid_test_vrml COMMAND $<TARGET_FILE:bvxm_grid_test_all>   test_bvxm_vrml )

add_executable( bvxm_grid_test_include test_include.cxx )
target_link_libraries( bvxm_grid_test_include bvxm )
add_executable( bvxm_grid_test_template_include test_template_include.cxx )
target_link_libraries( bvxm_grid_test_template_include bvxm )
