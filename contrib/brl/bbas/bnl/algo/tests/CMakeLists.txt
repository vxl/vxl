ADD_EXECUTABLE( bnl_algo_test_legendre  test_legendre_polynomial.cxx )

ADD_TEST(bnl_test_legendre_polynomial bnl_algo_test_legendre)

TARGET_LINK_LIBRARIES(bnl_algo_test_legendre bnl_algo bnl vnl)


ADD_EXECUTABLE( bnl_algo_test_integral test_integral.cxx)

ADD_TEST(bnl_test_integral bnl_algo_test_integral)

TARGET_LINK_LIBRARIES(bnl_algo_test_integral bnl_algo bnl vnl)
