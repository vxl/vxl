IF(BUILD_TESTING)
  # test
  ADD_EXECUTABLE( lbfgs-example lbfgs-example.c )
  ADD_TEST( netlib-lbfgs-example lbfgs-example )
  # test
  ADD_EXECUTABLE( lsqr-test lsqr-test.c )
  ADD_TEST( netlib-lsqr-test lsqr-test )
ENDIF(BUILD_TESTING)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNetlib.cmake )
