# This is core/vbl/io/tests/CMakeLists.txt

LINK_LIBRARIES( vbl_io testlib vpl )

ADD_EXECUTABLE( vbl_io_test_all
  test_driver.cxx
  vbl_io_test_classes.cxx vbl_io_test_classes.h
  test_smart_ptr_io.cxx
  test_bounding_box_io.cxx
  test_array_1d_io.cxx
  test_array_2d_io.cxx
  test_array_3d_io.cxx
  test_sparse_array_io.cxx
  test_triple_io.cxx
  golden_test_vbl_io.cxx
)

ADD_TEST(vbl_io_test_smart_ptr_io    vbl_io_test_all test_smart_ptr_io)
ADD_TEST(vbl_io_test_bounding_box_io vbl_io_test_all test_bounding_box_io)
ADD_TEST(vbl_io_test_array_1d_io     vbl_io_test_all test_array_1d_io)
ADD_TEST(vbl_io_test_array_2d_io     vbl_io_test_all test_array_2d_io)
ADD_TEST(vbl_io_test_array_3d_io     vbl_io_test_all test_array_3d_io)
ADD_TEST(vbl_io_test_sparse_array_io vbl_io_test_all test_sparse_array_io)
ADD_TEST(vbl_io_test_triple_io       vbl_io_test_all test_triple_io)
ADD_TEST(vbl_io_golden_test_vbl_io   vbl_io_test_all golden_test_vbl_io)

ADD_EXECUTABLE( vbl_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vbl_io_test_include vbl_io )
