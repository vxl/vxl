# This is core/vbl/tests/CMakeLists.txt


add_executable( vbl_test_all
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
target_link_libraries( vbl_test_all ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vbl_test_bounding_box COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_bounding_box )
add_test( NAME vbl_test_triple COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_triple )
add_test( NAME vbl_test_quadruple COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_quadruple )
add_test( NAME vbl_test_qsort COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_qsort )
add_test( NAME vbl_test_array COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_array )
add_test( NAME vbl_test_bit_array COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_bit_array )
add_test( NAME vbl_test_sparse_array_2d COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_sparse_array_2d )
add_test( NAME vbl_test_sparse_array_3d COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_sparse_array_3d )
add_test( NAME vbl_test_batch_multimap COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_batch_multimap )
add_test( NAME vbl_test_batch_compact_multimap COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_batch_compact_multimap )
add_test( NAME vbl_test_smart_ptr COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_smart_ptr )
add_test( NAME vbl_test_ref_count COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_ref_count )
add_test( NAME vbl_test_scoped_ptr COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_scoped_ptr )
add_test( NAME vbl_test_shared_pointer COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_shared_pointer )
add_test( NAME vbl_test_local_minima COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_local_minima )
add_test( NAME vbl_test_attributes COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_attributes )
add_test( NAME vbl_test_disjoint_sets COMMAND $<TARGET_FILE:vbl_test_all> vbl_test_disjoint_sets )

add_executable( vbl_test_include test_include.cxx )
target_link_libraries( vbl_test_include ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib)
add_executable( vbl_test_template_include test_template_include.cxx )
target_link_libraries( vbl_test_template_include ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib)
