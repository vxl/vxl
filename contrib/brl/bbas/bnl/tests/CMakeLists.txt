# brl/bbas/bnl/tests/CMakeLists.txt
# Tests for bnl library

ADD_EXECUTABLE( bnl_test_all
  test_driver.cxx
  test_crs_index.cxx
  test_sparse_lst_sqr_function.cxx
)

TARGET_LINK_LIBRARIES( bnl_test_all bnl testlib )

ADD_TEST( bnl_test_crs_index            bnl_test_all test_crs_index )
ADD_TEST( bnl_test_sparse_lst_sqr_func  bnl_test_all test_sparse_lst_sqr_function )

ADD_EXECUTABLE( bnl_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bnl_test_include bnl )
