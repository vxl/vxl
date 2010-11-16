ADD_EXECUTABLE( boxm2_test_all
  test_driver.cxx
  test_scene.cxx
  test_cache.cxx
  test_io.cxx
  test_wrappers.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_test_all testlib boxm2) # boct vnl vpgl vgl_xio vgl_algo vil vgl vpl vul)
ADD_TEST( boxm2_test_scene      ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_scene  )
ADD_TEST( boxm2_test_cache      ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_cache  )
ADD_TEST( boxm2_test_io         ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_io  )
ADD_TEST( boxm2_test_wrappers   ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all  test_wrappers  )

ADD_EXECUTABLE( boxm2_test_include test_include.cxx )
