add_executable( boxm_algo_sp_test_all
  test_driver.cxx
  test_compute_visibility.cxx
  test_render_image.cxx
  test_update.cxx
  test_update_multi_bin.cxx
 )

target_link_libraries( boxm_algo_sp_test_all boxm_algo_sp boxm_algo boxm boxm_util boxm_sample boxm_basic boct ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul)

add_test( NAME boxm_algo_sp_test_compute_visibility COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_compute_visibility  )
add_test( NAME boxm_algo_sp_test_render_image COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_render_image  )
if( VXL_RUN_FAILING_TESTS )
add_test( NAME boxm_algo_sp_test_update COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_update  )
endif()
add_test( NAME boxm_algo_sp_test_update_multi_bin COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_update_multi_bin  )

add_executable( boxm_algo_sp_test_include test_include.cxx )
target_link_libraries( boxm_algo_sp_test_include boxm_algo_sp)
