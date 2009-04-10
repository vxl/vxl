ADD_EXECUTABLE( boxm_test_all
  test_driver.cxx
  test_binary_io.cxx
  test_block_iter.cxx
  test_block_vis_graph.cxx
 )

TARGET_LINK_LIBRARIES( boxm_test_all boxm boct testlib vnl vgl_xio vpl vul vpgl)

ADD_TEST( boxm_test_binary_io   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_binary_io  )
ADD_TEST( boxm_test_block_iter   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_iter  )
ADD_TEST( boxm_block_vis_graph   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all block_vis_graph  )

ADD_EXECUTABLE( boxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_test_include boxm vul)
