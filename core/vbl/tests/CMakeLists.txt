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
  vbl_test_batch_multimap.cxx
  vbl_test_batch_compact_multimap.cxx
  vbl_test_smart_ptr.cxx
  vbl_test_ref_count.cxx
  vbl_test_scoped_ptr.cxx
  vbl_test_shared_pointer.cxx
  vbl_test_local_minima.cxx
  vbl_test_attributes.cxx
  vbl_test_disjoint_sets.cxx
)

ADD_TEST( NAME vbl_test_bounding_box COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_bounding_box )
ADD_TEST( NAME vbl_test_triple COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_triple )
ADD_TEST( NAME vbl_test_quadruple COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_quadruple )
ADD_TEST( NAME vbl_test_qsort COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_qsort )
ADD_TEST( NAME vbl_test_array COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_array )
ADD_TEST( NAME vbl_test_bit_array COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_bit_array )
ADD_TEST( NAME vbl_test_sparse_array_2d COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_sparse_array_2d )
ADD_TEST( NAME vbl_test_sparse_array_3d COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_sparse_array_3d )
ADD_TEST( NAME vbl_test_batch_multimap COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_batch_multimap )
ADD_TEST( NAME vbl_test_batch_compact_multimap COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_batch_compact_multimap )
ADD_TEST( NAME vbl_test_smart_ptr COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_smart_ptr )
ADD_TEST( NAME vbl_test_ref_count COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_ref_count )
ADD_TEST( NAME vbl_test_scoped_ptr COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_scoped_ptr )
ADD_TEST( NAME vbl_test_shared_pointer COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_shared_pointer )
ADD_TEST( NAME vbl_test_local_minima COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_local_minima )
ADD_TEST( NAME vbl_test_attributes COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_attributes )
ADD_TEST( NAME vbl_test_disjoint_sets COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_disjoint_sets )

ADD_EXECUTABLE( vbl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vbl_test_include vbl )
ADD_EXECUTABLE( vbl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vbl_test_template_include vbl )
