# brl/bbas/bnl/algo/tests/CMakeLists.txt
# Tests for bnl_algo library

ADD_EXECUTABLE ( bnl_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_sparse_lm.cxx
   test_legendre_polynomial.cxx
)

TARGET_LINK_LIBRARIES( bnl_algo_test_all bnl_algo testlib )


ADD_TEST( bnl_algo_test_sparse_lm     bnl_algo_test_all test_sparse_lm)
ADD_TEST( bnl_algo_test_legendre      bnl_algo_test_all test_legendre_polynomial)


ADD_EXECUTABLE( bnl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bnl_algo_test_include bnl_algo )





