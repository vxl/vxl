INCLUDE( ${allvxl_SOURCE_DIR}/rpl/rsdl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mbl/CMakeListsLink.txt )

ADD_EXECUTABLE( test_borgefors          test_borgefors  )
ADD_EXECUTABLE( test_rsdl_kd_tree       test_rsdl_kd_tree  )
ADD_EXECUTABLE( test_rsdl_point         test_rsdl_point  )
ADD_EXECUTABLE( test_rsdl_bounding_box  test_rsdl_bounding_box  )
ADD_EXECUTABLE( test_rsdl_bins_2d       test_rsdl_bins_2d  )
ADD_EXECUTABLE( test_rsdl_dist          test_rsdl_dist  )

ADD_TEST( test_borgefors          test_borgefors  )
ADD_TEST( test_rsdl_kd_tree       test_rsdl_kd_tree  )
ADD_TEST( test_rsdl_point         test_rsdl_point  )
ADD_TEST( test_rsdl_bounding_box  test_rsdl_bounding_box  )
ADD_TEST( test_rsdl_bins_2d       test_rsdl_bins_2d  )
ADD_TEST( test_rsdl_dist          test_rsdl_dist  )


INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/mul )


