IF(BUILD_TESTING)
  # test
  ADD_EXECUTABLE( lbfgs-example lbfgs-example.c )
  # test
  ADD_EXECUTABLE( lsqr-test lsqr-test.c )
ENDIF(BUILD_TESTING)


# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNetlib.cmake )
