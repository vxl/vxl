# vxl/vbl/io/tests/CMakeLists.txt

LINK_LIBRARIES( vbl_io testlib )

ADD_EXECUTABLE(vbl_test_io
  test_driver.cxx
  vbl_io_test_classes.cxx vbl_io_test_classes.h
  vbl_io_test_all.cxx
  test_smart_ptr_io.cxx
  test_bounding_box_io.cxx
  test_array_1d_io.cxx
  test_array_2d_io.cxx
  test_array_3d_io.cxx
  test_sparse_array_io.cxx
  golden_test_vbl_io.cxx
)

ADD_TEST(vbl_test_smart_ptr_io    vbl_test_io test_smart_ptr_io)
ADD_TEST(vbl_test_bounding_box_io vbl_test_io test_bounding_box_io)
ADD_TEST(vbl_test_array_1d_io     vbl_test_io test_array_1d_io)
ADD_TEST(vbl_test_array_2d_io     vbl_test_io test_array_2d_io)
ADD_TEST(vbl_test_array_3d_io     vbl_test_io test_array_3d_io)
ADD_TEST(vbl_test_sparse_array_io vbl_test_io test_sparse_array_io)
ADD_TEST(vbl_golden_test_vbl_io   vbl_test_io golden_test_vbl_io)
