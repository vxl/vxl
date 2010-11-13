ADD_EXECUTABLE( boxm2_test_all
  test_driver.cxx
  test_scene.cxx
 )
TARGET_LINK_LIBRARIES( boxm2_test_all testlib boxm2) # boct vnl vpgl vgl_xio vgl_algo vil vgl vpl vul)
ADD_TEST( test_scene               ${EXECUTABLE_OUTPUT_PATH}/boxm2_test_all    test_scene  )

ADD_EXECUTABLE( boxm2_test_include test_include.cxx )
