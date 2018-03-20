add_executable( boxm2_test_all
  test_driver.cxx
  test_scene.cxx
  test_cache.cxx
  test_cache2.cxx
  test_io.cxx
  test_wrappers.cxx
  test_data.cxx
  test_block.cxx
  test_block_vis_graph.cxx
  test_utils.h  test_utils.cxx
 )

target_link_libraries( boxm2_test_all ${VXL_LIB_PREFIX}testlib boxm2_cpp_pro brdb ${VXL_LIB_PREFIX}vpgl_algo vpgl_pro vil_pro sdet ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpl )

add_test( NAME boxm2_test_scene COMMAND $<TARGET_FILE:boxm2_test_all>  test_scene  )
add_test( NAME boxm2_test_cache COMMAND $<TARGET_FILE:boxm2_test_all>  test_cache  )
add_test( NAME boxm2_test_cache2 COMMAND $<TARGET_FILE:boxm2_test_all>  test_cache2  )
if( VXL_RUN_FAILING_TESTS ) ## These tests are always failing on Mac.  An infinite loop occurs in while statement
                                   ## due to failure in aio_read function on Mac.
add_test( NAME boxm2_test_io COMMAND $<TARGET_FILE:boxm2_test_all>  test_io  )
endif()
add_test( NAME boxm2_test_wrappers COMMAND $<TARGET_FILE:boxm2_test_all>  test_wrappers  )
add_test( NAME boxm2_test_data COMMAND $<TARGET_FILE:boxm2_test_all>  test_data  )
add_test( NAME boxm2_test_block COMMAND $<TARGET_FILE:boxm2_test_all>  test_block  )
add_test( NAME boxm2_test_block_vis_graph COMMAND $<TARGET_FILE:boxm2_test_all>  test_block_vis_graph  )

add_executable( boxm2_test_include test_include.cxx )
target_link_libraries( boxm2_test_include boxm2 boxm2_io boxm2_cpp )
add_executable( boxm2_test_template_include test_template_include.cxx )
target_link_libraries( boxm2_test_template_include boxm2 boxm2_io boxm2_cpp )
