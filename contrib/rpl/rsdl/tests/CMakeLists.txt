INCLUDE( ${allvxl_SOURCE_DIR}/rpl/rsdl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mbl/CMakeListsLink.txt )

ADD_EXECUTABLE( rsdl_test_borgefors     test_borgefors.cxx )
ADD_EXECUTABLE( test_rsdl_kd_tree       test_rsdl_kd_tree.cxx )
ADD_EXECUTABLE( test_rsdl_point         test_rsdl_point.cxx )
ADD_EXECUTABLE( test_rsdl_bounding_box  test_rsdl_bounding_box.cxx )
ADD_EXECUTABLE( test_rsdl_bins_2d       test_rsdl_bins_2d.cxx )
ADD_EXECUTABLE( test_rsdl_dist          test_rsdl_dist.cxx )

ADD_TEST( rsdl_test_borgefors rsdl_test_borgefors )
ADD_TEST( rsdl_test_rsdl_kd_tree       test_rsdl_kd_tree  )
ADD_TEST( rsdl_test_rsdl_point         test_rsdl_point  )
ADD_TEST( rsdl_test_rsdl_bounding_box  test_rsdl_bounding_box  )
ADD_TEST( rsdl_test_rsdl_bins_2d       test_rsdl_bins_2d  )
ADD_TEST( rsdl_test_rsdl_dist          test_rsdl_dist  )

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/mul )
