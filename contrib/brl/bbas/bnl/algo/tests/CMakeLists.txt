ADD_EXECUTABLE( bnl_algo_test_all  test_legendre_polynomial.cxx )

ADD_TEST(bnl_test_legendre_polynomial bnl_algo_test_all)

TARGET_LINK_LIBRARIES(bnl_algo_test_all bnl_algo bnl vnl)
