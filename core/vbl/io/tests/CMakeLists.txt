# vxl/vbl/io/tests/CMakeLists.txt

LINK_LIBRARIES( vbl_io testlib )

SET(test_vbl_io_sources
  test_vbl_io.cxx
  vbl_io_test_classes.h
)

ADD_EXECUTABLE(vbl_io_test_all vbl_io_test_all.cxx)
ADD_EXECUTABLE(test_vbl_io ${test_vbl_io_sources})
ADD_TEST(vbl_io_test_all vbl_io_test_all)
ADD_TEST(vbl_test_vbl_io test_vbl_io)
