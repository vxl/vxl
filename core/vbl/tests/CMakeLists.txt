# This is core/vbl/tests/CMakeLists.txt

LINK_LIBRARIES( vbl testlib )

ADD_EXECUTABLE( vbl_test_all
  test_driver.cxx
  vbl_test_classes.cxx vbl_test_classes.h
  vbl_test_array_x.cxx vbl_test_array_x.h
  vbl_test_array.cxx
  vbl_test_bit_array.cxx
  vbl_test_bounding_box.cxx
  vbl_test_triple.cxx
  vbl_test_quadruple.cxx
  vbl_test_qsort.cxx
  vbl_test_sparse_array_2d.cxx
  vbl_test_sparse_array_3d.cxx
  vbl_test_smart_ptr.cxx
)

ADD_TEST( vbl_test_bounding_box    vbl_test_all vbl_test_bounding_box )
ADD_TEST( vbl_test_triple          vbl_test_all vbl_test_triple )
ADD_TEST( vbl_test_quadruple       vbl_test_all vbl_test_quadruple )
ADD_TEST( vbl_test_qsort           vbl_test_all vbl_test_qsort )
ADD_TEST( vbl_test_array           vbl_test_all vbl_test_array )
ADD_TEST( vbl_test_bit_array       vbl_test_all vbl_test_bit_array )
ADD_TEST( vbl_test_sparse_array_2d vbl_test_all vbl_test_sparse_array_2d )
ADD_TEST( vbl_test_sparse_array_3d vbl_test_all vbl_test_sparse_array_3d )
ADD_TEST( vbl_test_smart_ptr       vbl_test_all vbl_test_smart_ptr )

ADD_EXECUTABLE( vbl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vbl_test_include vbl )
