add_executable( boxm_algo_test_all
  test_driver.cxx
  test_fill_in_mesh.cxx
  test_save_scene_raw.cxx
  test_upload_mesh.cxx
  test_boxm_fill_internal_cells.cxx
  test_boxm_scene_crop.cxx
  test_linear_operations.cxx
  test_remove_level0.cxx

 )

set(boxm_algo_test_all_sources
    test_utils.h    test_utils.cxx
    )

vxl_add_library(LIBRARY_NAME boxm_algo_tests LIBRARY_SOURCES  ${boxm_algo_test_all_sources})

target_link_libraries( boxm_algo_tests boxm_algo boxm boxm_sample boxm_util boxm_basic boct ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpgl imesh ${VXL_LIB_PREFIX}vcl)
target_link_libraries( boxm_algo_test_all boxm_algo_tests boxm_algo boxm boxm_sample boxm_util boxm_basic boct ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpgl imesh ${VXL_LIB_PREFIX}vcl)

add_test( NAME boxm_algo_test_fill_in_mesh COMMAND $<TARGET_FILE:boxm_algo_test_all> test_fill_in_mesh  )
add_test( NAME boxm_algo_test_save_scene_raw COMMAND $<TARGET_FILE:boxm_algo_test_all> test_save_scene_raw  )
add_test( NAME boxm_algo_test_upload_mesh COMMAND $<TARGET_FILE:boxm_algo_test_all> test_upload_mesh  )
add_test( NAME boxm_algo_test_fill_internal_cells COMMAND $<TARGET_FILE:boxm_algo_test_all> test_boxm_fill_internal_cells  )
add_test( NAME boxm_algo_test_scene_crop COMMAND $<TARGET_FILE:boxm_algo_test_all> test_boxm_scene_crop  )
add_test( NAME boxm_algo_test_linear_operations COMMAND $<TARGET_FILE:boxm_algo_test_all> test_linear_operations )
add_test( NAME boxm_algo_remove_level0 COMMAND $<TARGET_FILE:boxm_algo_test_all> test_remove_level0 )

add_executable( boxm_algo_test_include test_include.cxx )
target_link_libraries( boxm_algo_test_include boxm_algo)
