add_executable( bvxm_test_all
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

target_link_libraries( bvxm_test_all bvxm bvxm_grid ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpgl bsta bsta_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl )

add_test( NAME bvxm_test_apm_processors COMMAND $<TARGET_FILE:bvxm_test_all>   test_apm_processors )
add_test( NAME bvxm_test_lidar_processor COMMAND $<TARGET_FILE:bvxm_test_all>   test_lidar_processor )
add_test( NAME bvxm_test_voxel_world COMMAND $<TARGET_FILE:bvxm_test_all>   test_voxel_world )
add_test( NAME bvxm_test_voxel_world_update COMMAND $<TARGET_FILE:bvxm_test_all>   test_voxel_world_update )
add_test( NAME bvxm_test_voxel_world_mog_image COMMAND $<TARGET_FILE:bvxm_test_all>   test_voxel_world_mog_image )
add_test( NAME bvxm_test_platform_computations COMMAND $<TARGET_FILE:bvxm_test_all>   test_platform_computations )
add_test( NAME bvxm_test_tangent_update COMMAND $<TARGET_FILE:bvxm_test_all>   test_tangent_update )
add_test( NAME bvxm_test_illum COMMAND $<TARGET_FILE:bvxm_test_all>   test_illum )

add_executable( bvxm_test_include test_include.cxx )
target_link_libraries( bvxm_test_include bvxm bvxm_io bvxm_grid )
add_executable( bvxm_test_template_include test_template_include.cxx )
target_link_libraries( bvxm_test_template_include bvxm bvxm_io bvxm_grid )
