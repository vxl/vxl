ADD_EXECUTABLE( bvxm_algo_test_all
  test_driver.cxx
  test_merge_mog.cxx
  test_mog_norm.cxx
# causes memory allocation errors (FIX_ME)
#  test_boxm_scene_to_bvxm_grid.cxx 
)

TARGET_LINK_LIBRARIES( bvxm_algo_test_all bvxm_algo bvxm_grid testlib bsta)

ADD_TEST( bvxm_algo_test_merge_mog               ${EXECUTABLE_OUTPUT_PATH}/bvxm_algo_test_all   test_merge_mog )
ADD_TEST( bvxm_algo_test_mog_norm                ${EXECUTABLE_OUTPUT_PATH}/bvxm_algo_test_all   test_mog_norm )
#ADD_TEST( bvxm_algo_test_boxm_scene_to_bvxm_grid ${EXECUTABLE_OUTPUT_PATH}/bvxm_algo_test_all   test_boxm_scene_to_bvxm_grid)

ADD_EXECUTABLE( bvxm_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_algo_test_include bvxm_algo)
ADD_EXECUTABLE( bvxm_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_algo_test_template_include bvxm_algo)
