ADD_EXECUTABLE( boxm_algo_sp_test_all
  test_driver.cxx
  test_compute_visibility.cxx
  test_render_image.cxx
  test_update.cxx
  test_update_multi_bin.cxx
 )

TARGET_LINK_LIBRARIES( boxm_algo_sp_test_all boxm_algo_sp boxm_algo boxm boxm_util boxm_sample boxm_basic boct testlib vgl vil vpl vul)

ADD_TEST( NAME boxm_algo_sp_test_compute_visibility COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_compute_visibility  )
ADD_TEST( NAME boxm_algo_sp_test_render_image COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_render_image  )
if( HACK_FORCE_BRL_FAILING_TESTS )
ADD_TEST( NAME boxm_algo_sp_test_update COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_update  )
endif()
ADD_TEST( NAME boxm_algo_sp_test_update_multi_bin COMMAND $<TARGET_FILE:boxm_algo_sp_test_all> test_update_multi_bin  )

ADD_EXECUTABLE( boxm_algo_sp_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_algo_sp_test_include boxm_algo_sp)
