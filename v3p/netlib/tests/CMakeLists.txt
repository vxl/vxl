IF(BUILD_TESTING)
  # test
  ADD_EXECUTABLE( netlib_lbfgs-example lbfgs-example.c )
  ADD_TEST( netlib-lbfgs-example netlib_lbfgs-example )
  # test
  ADD_EXECUTABLE( netlib_lsqr-test lsqr-test.c )
  ADD_TEST( netlib-lsqr-test netlib_lsqr-test )
ENDIF(BUILD_TESTING)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNetlib.cmake )
