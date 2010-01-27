INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
ADD_EXECUTABLE( boxm_algo_test_all
  test_driver.cxx
  test_boxm_scene_crop.cxx
  test_boxm_scene_to_bvxm_grid.cxx
 )

TARGET_LINK_LIBRARIES( boxm_algo_test_all boxm_algo testlib vul)

ADD_TEST( test_boxm_scene_crop   ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_boxm_scene_crop  )
ADD_TEST(test_boxm_scene_to_bvxm_grid ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_boxm_scene_to_bvxm_grid )

ADD_EXECUTABLE( boxm_algo_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( boxm_algo_test_all boxm_algo bvxm_grid)
