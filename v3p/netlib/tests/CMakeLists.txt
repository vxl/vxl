IF(BUILD_TESTING)
  LINK_LIBRARIES( netlib )

  # test
  ADD_EXECUTABLE( netlib_lbfgs_example lbfgs-example.c )
  ADD_TEST( netlib_lbfgs_example netlib_lbfgs_example )
  # test
  ADD_EXECUTABLE( netlib_lsqr_test lsqr-test.c )
  ADD_TEST( netlib_lsqr_test netlib_lsqr_test )
ENDIF(BUILD_TESTING)
