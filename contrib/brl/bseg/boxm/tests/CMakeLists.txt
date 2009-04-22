ADD_EXECUTABLE( boxm_test_all
  test_driver.cxx
  test_binary_io.cxx
  test_block_iter.cxx
  test_block_vis_graph.cxx
  test_cell_vis_graph.cxx

  test_utils.h test_utils.cxx
  test_quad_interpolate.cxx
  test_render_image.cxx
  test_save_raw.cxx
  test_update.cxx
 )

TARGET_LINK_LIBRARIES( boxm_test_all boxm boct testlib vnl vgl_xio vpl vul vpgl)

ADD_TEST( boxm_test_binary_io   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_binary_io  )
ADD_TEST( boxm_test_block_iter   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_iter  )
ADD_TEST( boxm_block_vis_graph   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_vis_graph  )
ADD_TEST( boxm_test_cell_vis_graph   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_cell_vis_graph  )
ADD_TEST( boxm_test_quad_interpolate   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_quad_interpolate  )
ADD_TEST( boxm_test_render_image   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_render_image  )
ADD_TEST( boxm_test_save_raw   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_save_raw  )
ADD_TEST( boxm_test_update   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_update  )


ADD_EXECUTABLE( boxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_test_include boxm boct)
