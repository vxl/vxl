ADD_EXECUTABLE( boxm2_basic_test_all
  test_driver.cxx
  test_block_vis_graph.cxx
  test_cell_vis_graph.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_basic_test_all boxm2_basic boxm2 boxm2_util testlib vpl vul)

ADD_TEST( boxm2_test_block_vis_graph    ${EXECUTABLE_OUTPUT_PATH}/boxm2_basic_test_all test_block_vis_graph  )
ADD_TEST( boxm2_test_cell_vis_graph   ${EXECUTABLE_OUTPUT_PATH}/boxm2_basic_test_all test_cell_vis_graph  )

ADD_EXECUTABLE( boxm2_basic_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_basic_test_include boxm2_basic)
