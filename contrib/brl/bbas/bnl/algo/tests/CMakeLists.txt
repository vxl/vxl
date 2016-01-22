# brl/bbas/bnl/algo/tests/CMakeLists.txt
# Tests for bnl_algo library

ADD_EXECUTABLE ( bnl_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_legendre_polynomial.cxx
   test_fresnel.cxx
)

TARGET_LINK_LIBRARIES( bnl_algo_test_all bnl_algo vnl testlib )

ADD_TEST( bnl_algo_test_legendre_polynomial ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bnl_algo_test_all test_legendre_polynomial)
ADD_TEST( bnl_test_fresnel ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bnl_algo_test_all test_fresnel )

ADD_EXECUTABLE( bnl_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bnl_algo_test_include bnl_algo )
