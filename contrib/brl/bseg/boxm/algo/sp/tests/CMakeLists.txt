ADD_EXECUTABLE( boxm_algo_sp_test_all
  test_driver.cxx
  test_compute_visibility.cxx
  test_render_image.cxx
  test_update.cxx
  test_update_multi_bin.cxx
 )

TARGET_LINK_LIBRARIES( boxm_algo_sp_test_all boxm_algo_sp boxm_algo boxm boxm_util boxm_sample boxm_basic boct testlib vgl vil vpl vul)

ADD_TEST( boxm_algo_sp_test_compute_visibility ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_sp_test_all test_compute_visibility  )
ADD_TEST( boxm_algo_sp_test_render_image       ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_sp_test_all test_render_image  )
ADD_TEST( boxm_algo_sp_test_update             ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_sp_test_all test_update  )
ADD_TEST( boxm_algo_sp_test_update_multi_bin   ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_sp_test_all test_update_multi_bin  )

ADD_EXECUTABLE( boxm_algo_sp_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_algo_sp_test_include boxm_algo_sp)
