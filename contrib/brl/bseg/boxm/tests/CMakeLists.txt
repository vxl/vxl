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
  test_update_multi_bin.cxx
  test_compute_visibility.cxx
  test_boxm_rational_camera_utils.cxx
  test_upload_mesh.cxx
  test_fill_in_mesh.cxx
  test_save_scene_raw.cxx
 )

TARGET_LINK_LIBRARIES( boxm_test_all boxm boct testlib vnl vgl_xio vpl vul vpgl)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

ADD_TEST( boxm_test_binary_io          ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_binary_io  )
ADD_TEST( boxm_test_block_iter         ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_iter  )
ADD_TEST( boxm_block_vis_graph         ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_block_vis_graph  ) 
ADD_TEST( boxm_test_cell_vis_graph     ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_cell_vis_graph  )
ADD_TEST( boxm_test_quad_interpolate   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_quad_interpolate ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )
ADD_TEST( boxm_test_render_image       ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_render_image  )
ADD_TEST( boxm_test_save_raw           ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_save_raw  )
ADD_TEST( boxm_test_update             ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_update  )
ADD_TEST( boxm_test_update_multi_bin   ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_update_multi_bin  )
ADD_TEST( boxm_test_compute_visibility ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_compute_visibility  )
ADD_TEST( boxm_test_boxm_rational_camera_utils ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_boxm_rational_camera_utils ${CMAKE_CURRENT_SOURCE_DIR} )
ADD_TEST( boxm_test_upload_mesh ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_upload_mesh  )
ADD_TEST( boxm_test_fill_in_mesh ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_fill_in_mesh  )
ADD_TEST( boxm_test_save_scene_raw     ${EXECUTABLE_OUTPUT_PATH}/boxm_test_all test_save_scene_raw  )

ADD_EXECUTABLE( boxm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_test_include boxm boct)
