include_directories( ${MUL_INCLUDE_DIR} )

add_executable( rsdl_test_all
  test_driver.cxx

  test_bins_2d.cxx
  test_borgefors.cxx
  test_bounding_box.cxx
  test_dist.cxx
  test_kd_tree.cxx
  test_point.cxx
  test_bins.cxx
)
target_link_libraries( rsdl_test_all rsdl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl )

add_test( NAME rsdl_test_bins_2d COMMAND $<TARGET_FILE:rsdl_test_all> test_bins_2d )
add_test( NAME rsdl_test_borgefors COMMAND $<TARGET_FILE:rsdl_test_all> test_borgefors )
add_test( NAME rsdl_test_bounding_box COMMAND $<TARGET_FILE:rsdl_test_all> test_bounding_box )
add_test( NAME rsdl_test_dist COMMAND $<TARGET_FILE:rsdl_test_all> test_dist )
add_test( NAME rsdl_test_kd_tree COMMAND $<TARGET_FILE:rsdl_test_all> test_kd_tree )
add_test( NAME rsdl_test_point COMMAND $<TARGET_FILE:rsdl_test_all> test_point )
add_test( NAME rsdl_test_bins COMMAND $<TARGET_FILE:rsdl_test_all> test_bins )

add_executable( rsdl_test_include test_include.cxx )
target_link_libraries( rsdl_test_include rsdl )
add_executable( rsdl_test_template_include test_template_include.cxx )
target_link_libraries( rsdl_test_template_include rsdl )
