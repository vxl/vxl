ADD_EXECUTABLE( vdgl_test_all
  test_driver.cxx
  test_vdgl.cxx
  test1_vdgl.cxx
  test_vdgl_io.cxx
)
TARGET_LINK_LIBRARIES( vdgl_test_all vsol vdgl vbl_io vsl vpl testlib )

ADD_TEST(vdgl_test_vdgl  vdgl_test_all test_vdgl)
ADD_TEST(vdgl_test_vdgl1 vdgl_test_all test1_vdgl)
ADD_TEST(vdgl_test_io    vdgl_test_all test_vdgl_io)

ADD_EXECUTABLE( vdgl_test_include test_include.cxx )
