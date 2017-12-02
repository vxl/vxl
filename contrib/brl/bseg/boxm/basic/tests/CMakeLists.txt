add_executable( boxm_basic_test_all
  test_driver.cxx
  test_block_vis_graph.cxx
  test_cell_vis_graph.cxx
 )

target_link_libraries( boxm_basic_test_all boxm_basic boxm boxm_util ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul)

add_test( NAME boxm_basic_test_block_vis_graph COMMAND $<TARGET_FILE:boxm_basic_test_all> test_block_vis_graph  )
add_test( NAME boxm_basic_test_cell_vis_graph COMMAND $<TARGET_FILE:boxm_basic_test_all> test_cell_vis_graph  )

add_executable( boxm_basic_test_include test_include.cxx )
target_link_libraries( boxm_basic_test_include boxm_basic)
add_executable( boxm_basic_test_template_include test_template_include.cxx )
target_link_libraries( boxm_basic_test_template_include boxm_basic)
