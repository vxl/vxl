ADD_EXECUTABLE( bnl_algo_test_legendre  test_legendre_polynomial.cxx )
TARGET_LINK_LIBRARIES(bnl_algo_test_legendre vcl vnl bnl bnl_algo)

ADD_EXECUTABLE( bnl_algo_test_fst       test_fst.cxx)
TARGET_LINK_LIBRARIES(bnl_algo_test_fst vcl vnl bnl bnl_algo)

ADD_TEST(bnl_algo_test 
         bnl_algo_test_legendre 
         bnl_algo_test_fst
        )



