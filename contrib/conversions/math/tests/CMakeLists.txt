INCLUDE( ${allvxl_SOURCE_DIR}/conversions/math/CMakeListsLink.txt )

ADD_EXECUTABLE( test_matrix_conversions test_matrix_conversions.cxx )
ADD_TEST( conversions_test_matrix_conversions test_matrix_conversions )

IF(HAS_COOL)
  ADD_EXECUTABLE( test_CoolMatrix_conversions test_CoolMatrix_conversions.cxx )
  ADD_TEST( conversions_test_CoolMatrix_conversions test_CoolMatrix_conversions )
ENDIF(HAS_COOL)

IF(HAS_NUMERICS)
  ADD_EXECUTABLE( test_IUE_matrix_conversions test_IUE_matrix_conversions.cxx )
  ADD_TEST( conversions_test_IUE_matrix_conversions test_IUE_matrix_conversions )
ENDIF(HAS_NUMERICS)
