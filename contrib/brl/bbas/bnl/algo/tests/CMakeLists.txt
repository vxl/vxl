
CREATE_TEST_SOURCELIST(bnl_algo_test_src test_drive.cxx
                       test_legendre_polynomial.cxx
                       )
                       
ADD_EXECUTABLE( bnl_algo_test_all ${bnl_algo_test_src})

ADD_TEST(test_legendre_polynomial bnl_algo_test_all test_legendre_polynomial)

TARGET_LINK_LIBRARIES( bnl_algo_test_all bnl_algo bnl vnl)

