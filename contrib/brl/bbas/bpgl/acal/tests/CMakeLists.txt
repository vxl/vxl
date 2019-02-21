# This is acal/tests/CMakeLists.txt
add_executable( acal_test_all
  test_driver.cxx
  test_f_utils.cxx
  test_metadata.cxx
  test_match_utils.cxx
  test_match_tree.cxx
)

target_link_libraries( acal_test_all ${VXL_LIB_PREFIX}acal ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME acal_test_f_utils COMMAND $<TARGET_FILE:acal_test_all> test_f_utils )
add_test( NAME acal_test_metadata COMMAND $<TARGET_FILE:acal_test_all> test_metadata )
add_test( NAME acal_test_match_utils COMMAND $<TARGET_FILE:acal_test_all> test_match_utils )
add_test( NAME acal_test_match_tree COMMAND $<TARGET_FILE:acal_test_all> test_match_tree )

