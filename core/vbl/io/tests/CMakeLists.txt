# This is core/vbl/io/tests/CMakeLists.txt

link_libraries( ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl )

add_executable( vbl_io_test_all
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

add_test( NAME vbl_io_test_smart_ptr_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_smart_ptr_io)
add_test( NAME vbl_io_test_bounding_box_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_bounding_box_io)
add_test( NAME vbl_io_test_array_1d_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_array_1d_io)
add_test( NAME vbl_io_test_array_2d_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_array_2d_io)
add_test( NAME vbl_io_test_array_3d_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_array_3d_io)
add_test( NAME vbl_io_test_sparse_array_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_sparse_array_io)
add_test( NAME vbl_io_test_triple_io COMMAND $<TARGET_FILE:vbl_io_test_all> test_triple_io)
add_test( NAME vbl_io_golden_test_vbl_io COMMAND $<TARGET_FILE:vbl_io_test_all> golden_test_vbl_io)

add_executable( vbl_io_test_include test_include.cxx )
target_link_libraries( vbl_io_test_include ${VXL_LIB_PREFIX}vbl_io )
add_executable( vbl_io_test_template_include test_template_include.cxx )
target_link_libraries( vbl_io_test_template_include ${VXL_LIB_PREFIX}vbl_io )
