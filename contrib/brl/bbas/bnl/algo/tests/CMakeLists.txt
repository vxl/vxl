ADD_EXECUTABLE( bnl_algo_test_legendre  test_legendre_polynomial.cxx )
TARGET_LINK_LIBRARIES(bnl_algo_test_legendre vcl vnl bnl_algo)

ADD_TEST(bnl_algo_test 
         ${EXECUTABLE_OUTPUT_PATH}/bnl_algo_test_legendre 
        )



