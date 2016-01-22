INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( rsdl_test_all
  test_driver.cxx

  test_bins_2d.cxx
  test_borgefors.cxx
  test_bounding_box.cxx
  test_dist.cxx
  test_kd_tree.cxx
  test_point.cxx
  test_bins.cxx
)
TARGET_LINK_LIBRARIES( rsdl_test_all rsdl testlib vnl )

ADD_TEST( NAME rsdl_test_bins_2d COMMAND $<TARGET_FILE:rsdl_test_all> test_bins_2d )
ADD_TEST( NAME rsdl_test_borgefors COMMAND $<TARGET_FILE:rsdl_test_all> test_borgefors )
ADD_TEST( NAME rsdl_test_bounding_box COMMAND $<TARGET_FILE:rsdl_test_all> test_bounding_box )
ADD_TEST( NAME rsdl_test_dist COMMAND $<TARGET_FILE:rsdl_test_all> test_dist )
ADD_TEST( NAME rsdl_test_kd_tree COMMAND $<TARGET_FILE:rsdl_test_all> test_kd_tree )
ADD_TEST( NAME rsdl_test_point COMMAND $<TARGET_FILE:rsdl_test_all> test_point )
ADD_TEST( NAME rsdl_test_bins COMMAND $<TARGET_FILE:rsdl_test_all> test_bins )

ADD_EXECUTABLE( rsdl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( rsdl_test_include rsdl )
ADD_EXECUTABLE( rsdl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( rsdl_test_template_include rsdl )
