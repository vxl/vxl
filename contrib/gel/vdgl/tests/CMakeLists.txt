LINK_LIBRARIES( vdgl testlib )

ADD_EXECUTABLE( vdgl_test_all
  test_driver.cxx
  test_vdgl.cxx
  test1_vdgl.cxx
)

ADD_TEST(test_vdgl  vdgl_test_all test_vdgl)
ADD_TEST(test1_vdgl vdgl_test_all test1_vdgl)

ADD_EXECUTABLE( vdgl_test_include test_include.cxx )
