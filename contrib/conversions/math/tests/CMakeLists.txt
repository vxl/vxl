LINK_LIBRARIES( vnl vbl testlib )

SET(HAS_COOL "NO")
IF(EXISTS ${vxl_SOURCE_DIR}/COOL/CMakeLists.txt )
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/COOL)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/Numerics)
  LINK_LIBRARIES( TJCOOL )
  SET(HAS_COOL "YES")
ENDIF( EXISTS ${vxl_SOURCE_DIR}/COOL/CMakeLists.txt )

SET(HAS_NUMERICS "NO")
IF(EXISTS ${vxl_SOURCE_DIR}/Numerics/CMakeLists.txt )
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/Numerics)
  LINK_LIBRARIES( Numerics iue-math )
  SET(HAS_NUMERICS "YES")
ENDIF(EXISTS ${vxl_SOURCE_DIR}/Numerics/CMakeLists.txt )

IF(HAS_COOL)
  ADD_DEFINITIONS(-DHAS_COOL=1)
ELSE(HAS_COOL)
  ADD_DEFINITIONS(-DHAS_NO_COOL=1)
ENDIF(HAS_COOL)

IF(HAS_NUMERICS)
  ADD_DEFINITIONS(-DHAS_NUMERICS=1)
ELSE(HAS_NUMERICS)
  ADD_DEFINITIONS(-DHAS_NO_NUMERICS=1)
ENDIF(HAS_NUMERICS)

ADD_EXECUTABLE( conversions_math_test_all
  test_driver.cxx

  test_matrix_conversions.cxx
  test_CoolMatrix_conversions.cxx
  test_IUE_matrix_conversions.cxx
)

ADD_TEST( conversions_test_matrix_conversions conversions_math_test_all test_matrix_conversions )
ADD_TEST( conversions_test_CoolMatrix_conversions conversions_math_test_all test_CoolMatrix_conversions )
ADD_TEST( conversions_test_IUE_matrix_conversions conversions_math_test_all test_IUE_matrix_conversions )

ADD_EXECUTABLE( conversions_math_test_include test_include.cxx )
