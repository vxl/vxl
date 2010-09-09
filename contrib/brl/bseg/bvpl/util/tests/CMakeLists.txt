
ADD_EXECUTABLE( bvpl_util_test_all
  test_driver.cxx
  test_corner_finder.cxx
 )

TARGET_LINK_LIBRARIES( bvpl_util_test_all testlib bvpl_util bvxm_grid_io vgl vul)

ADD_TEST( bvpl_util_test_corner_finder   ${EXECUTABLE_OUTPUT_PATH}/bvpl_util_test_all test_corner_finder  )

ADD_EXECUTABLE( bvpl_util_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_util_test_include bvpl_util )
