add_executable( bvxm_algo_test_all
  test_driver.cxx
  test_merge_mog.cxx
  test_mog_norm.cxx
)

target_link_libraries( bvxm_algo_test_all bvxm_algo bvxm_grid ${VXL_LIB_PREFIX}testlib bsta)

add_test( NAME bvxm_algo_test_merge_mog COMMAND $<TARGET_FILE:bvxm_algo_test_all>   test_merge_mog )
add_test( NAME bvxm_algo_test_mog_norm COMMAND $<TARGET_FILE:bvxm_algo_test_all>   test_mog_norm )

add_executable( bvxm_algo_test_include test_include.cxx )
target_link_libraries( bvxm_algo_test_include bvxm_algo)
add_executable( bvxm_algo_test_template_include test_template_include.cxx )
target_link_libraries( bvxm_algo_test_template_include bvxm_algo)
