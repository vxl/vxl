ADD_EXECUTABLE( boxm_basic_test_all
  test_driver.cxx
  test_block_vis_graph.cxx
  test_cell_vis_graph.cxx
 )

TARGET_LINK_LIBRARIES( boxm_basic_test_all boxm_basic boxm boxm_util testlib vpl vul)

ADD_TEST( boxm_basic_test_block_vis_graph  ${EXECUTABLE_OUTPUT_PATH}/boxm_basic_test_all test_block_vis_graph  )
ADD_TEST( boxm_basic_test_cell_vis_graph   ${EXECUTABLE_OUTPUT_PATH}/boxm_basic_test_all test_cell_vis_graph  )

ADD_EXECUTABLE( boxm_basic_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_basic_test_include boxm_basic)
ADD_EXECUTABLE( boxm_basic_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( boxm_basic_test_template_include boxm_basic)
