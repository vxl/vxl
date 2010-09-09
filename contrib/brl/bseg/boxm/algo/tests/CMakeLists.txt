ADD_EXECUTABLE( boxm_algo_test_all
  test_driver.cxx
  test_fill_in_mesh.cxx
  test_save_scene_raw.cxx
  test_upload_mesh.cxx
  test_boxm_fill_internal_cells.cxx
  test_boxm_scene_crop.cxx
 )

TARGET_LINK_LIBRARIES( boxm_algo_test_all boxm_algo boxm boxm_sample boxm_util boxm_basic boct testlib vnl_io vpgl vgl_algo vgl vpl vul vpgl imesh vcl)

ADD_TEST( boxm_algo_test_fill_in_mesh        ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_fill_in_mesh  )
ADD_TEST( boxm_algo_test_save_scene_raw      ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_save_scene_raw  )
ADD_TEST( boxm_algo_test_upload_mesh         ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_upload_mesh  )
ADD_TEST( boxm_algo_test_fill_internal_cells ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_boxm_fill_internal_cells  )
ADD_TEST( boxm_algo_test_scene_crop          ${EXECUTABLE_OUTPUT_PATH}/boxm_algo_test_all test_boxm_scene_crop  )

ADD_EXECUTABLE( boxm_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_algo_test_include boxm_algo)
