ADD_EXECUTABLE( boxm2_test_all
  test_driver.cxx
  test_scene.cxx
  test_cache.cxx
  test_io.cxx
  test_wrappers.cxx
  test_data.cxx
  test_block.cxx
  test_cpp_render_process.cxx
  test_cpp_update_process.cxx
  test_utils.h  test_utils.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_test_all testlib boxm2_cpp_pro   brdb vpgl_algo vpgl_pro vil_pro sdet vul vpl )

ADD_TEST( boxm2_test_scene               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_scene  )
ADD_TEST( boxm2_test_cache               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_cache  )
ADD_TEST( boxm2_test_io                  ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_io  )
ADD_TEST( boxm2_test_wrappers            ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_wrappers  )
ADD_TEST( boxm2_test_data                ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_data  )
ADD_TEST( test_block                     ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_block  )
ADD_TEST( boxm2_test_cpp_render_process  ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_cpp_render_process  )
ADD_TEST( boxm2_test_cpp_update_process  ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_cpp_update_process  )

ADD_EXECUTABLE( boxm2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_test_include boxm2 boxm2_io boxm2_cpp )
