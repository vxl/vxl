LINK_LIBRARIES( rsdl mbl testlib )

INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( rsdl_test_driver
  test_driver.cxx

  test_bins_2d.cxx
  test_borgefors.cxx
  test_bounding_box.cxx
  test_dist.cxx
  test_kd_tree.cxx
  test_point.cxx
)

ADD_TEST( rsdl_test_bins_2d       rsdl_test_driver test_bins_2d )
ADD_TEST( rsdl_test_borgefors     rsdl_test_driver test_borgefors )
ADD_TEST( rsdl_test_bounding_box  rsdl_test_driver test_bounding_box )
ADD_TEST( rsdl_test_dist          rsdl_test_driver test_dist )
ADD_TEST( rsdl_test_kd_tree       rsdl_test_driver test_kd_tree )
ADD_TEST( rsdl_test_point         rsdl_test_driver test_point )
