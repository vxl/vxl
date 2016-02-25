# brl/bbas/bnl/algo/tests/CMakeLists.txt
# Tests for bnl_algo library

add_executable( bnl_algo_test_all
   # Driver source and utilities
   test_driver.cxx

   # The tests
   test_legendre_polynomial.cxx
   test_fresnel.cxx
)

target_link_libraries( bnl_algo_test_all bnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}testlib )

add_test( NAME bnl_algo_test_legendre_polynomial COMMAND $<TARGET_FILE:bnl_algo_test_all> test_legendre_polynomial)
add_test( NAME bnl_test_fresnel COMMAND $<TARGET_FILE:bnl_algo_test_all> test_fresnel )

add_executable( bnl_algo_test_include test_include.cxx )
target_link_libraries( bnl_algo_test_include bnl_algo )
