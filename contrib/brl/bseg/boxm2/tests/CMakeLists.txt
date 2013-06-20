ADD_EXECUTABLE( boxm2_test_all
  test_driver.cxx
  test_scene.cxx
  test_cache.cxx
  test_cache2.cxx
  test_io.cxx
  test_wrappers.cxx
  test_data.cxx
  test_block.cxx
  test_utils.h  test_utils.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_test_all testlib boxm2_cpp_pro brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vul vpl )

ADD_TEST( boxm2_test_scene               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_scene  )
ADD_TEST( boxm2_test_cache               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_cache  )
ADD_TEST( boxm2_test_cache2               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_cache2  )
ADD_TEST( boxm2_test_io                  ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_io  )
ADD_TEST( boxm2_test_wrappers            ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_wrappers  )
ADD_TEST( boxm2_test_data                ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_data  )
ADD_TEST( boxm2_test_block               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_block  )

ADD_EXECUTABLE( boxm2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_test_include boxm2 boxm2_io boxm2_cpp )
ADD_EXECUTABLE( boxm2_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_test_template_include boxm2 boxm2_io boxm2_cpp )
