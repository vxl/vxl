LINK_LIBRARIES(vdgl vpl testlib )

ADD_EXECUTABLE( vdgl_test_all
  test_driver.cxx
  test_vdgl.cxx
  test1_vdgl.cxx
  test_vdgl_io.cxx
)

ADD_TEST(vdgl_test_vdgl  vdgl_test_all test_vdgl)
ADD_TEST(vdgl_test_vdgl1 vdgl_test_all test1_vdgl)
ADD_TEST(test_vdgl_io    vdgl_test_all test_vdgl_io)

ADD_EXECUTABLE( vdgl_test_include test_include.cxx )
