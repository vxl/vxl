ADD_EXECUTABLE( boxm2_algo_sp_test_all
  test_driver.cxx
  test_compute_visibility.cxx
  test_render_image.cxx
  test_update.cxx
  test_update_multi_bin.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_algo_sp_test_all boxm2_algo_sp boxm2_algo boxm2 boxm2_util boxm2_sample boxm2_basic boct testlib vpl vul)

ADD_TEST( boxm_test_compute_visibility ${EXECUTABLE_OUTPUT_PATH}/boxm2_algo_test_all test_compute_visibility  )
ADD_TEST( boxm_test_render_image ${EXECUTABLE_OUTPUT_PATH}/boxm2_algo_test_all test_render_image  )
ADD_TEST( boxm_test_update ${EXECUTABLE_OUTPUT_PATH}/boxm2_algo_test_all test_update  )
ADD_TEST( boxm_test_update_multi_bin ${EXECUTABLE_OUTPUT_PATH}/boxm2_algo_test_all test_update_multi_bin  )

ADD_EXECUTABLE( boxm2_algo_sp_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_algo_sp_test_include boxm2_algo_sp)
