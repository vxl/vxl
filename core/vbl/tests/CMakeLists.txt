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
  vbl_test_ref_count.cxx
  vbl_test_scoped_ptr.cxx
  vbl_test_shared_pointer.cxx
)

ADD_TEST( vbl_test_bounding_box    ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_bounding_box )
ADD_TEST( vbl_test_triple          ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_triple )
ADD_TEST( vbl_test_quadruple       ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_quadruple )
ADD_TEST( vbl_test_qsort           ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_qsort )
ADD_TEST( vbl_test_array           ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_array )
ADD_TEST( vbl_test_bit_array       ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_bit_array )
ADD_TEST( vbl_test_sparse_array_2d ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_sparse_array_2d )
ADD_TEST( vbl_test_sparse_array_3d ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_sparse_array_3d )
ADD_TEST( vbl_test_smart_ptr       ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_smart_ptr )
ADD_TEST( vbl_test_ref_count       ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_ref_count )
ADD_TEST( vbl_test_scoped_ptr      ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_scoped_ptr )
ADD_TEST( vbl_test_shared_pointer      ${EXECUTABLE_OUTPUT_PATH}/vbl_test_all vbl_test_shared_ointer )

ADD_EXECUTABLE( vbl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vbl_test_include vbl )
