INCLUDE( ${CONVERSIONS_INCLUDE_DIR}/math/CMakeListsLink.txt )
LINK_LIBRARIES( testlib )

IF(HAS_COOL)
ELSE(HAS_COOL)
  ADD_DEFINITIONS(-DHAS_NO_COOL=1)
ENDIF(HAS_COOL)

IF(HAS_NUMERICS)
ELSE(HAS_NUMERICS)
  ADD_DEFINITIONS(-DHAS_NO_NUMERICS=1)
ENDIF(HAS_NUMERICS)

ADD_EXECUTABLE( conversions_math_test_driver
  test_driver.cxx

  test_matrix_conversions.cxx
  test_CoolMatrix_conversions.cxx
  test_IUE_matrix_conversions.cxx
)

ADD_TEST( conversions_test_matrix_conversions conversions_math_test_driver test_matrix_conversions )
ADD_TEST( conversions_test_CoolMatrix_conversions conversions_math_test_driver test_CoolMatrix_conversions )
ADD_TEST( conversions_test_IUE_matrix_conversions conversions_math_test_driver test_IUE_matrix_conversions )
