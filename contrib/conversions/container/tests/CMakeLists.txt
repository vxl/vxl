LINK_LIBRARIES( vnl testlib )

SET(HAS_COOL "NO")
IF(EXISTS ${vxl_SOURCE_DIR}/COOL/CMakeLists.txt )
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/COOL)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/Numerics)
  LINK_LIBRARIES( TJCOOL )
  SET(HAS_COOL "YES")
ENDIF( EXISTS ${vxl_SOURCE_DIR}/COOL/CMakeLists.txt )

IF(HAS_COOL)
  ADD_DEFINITIONS(-DHAS_COOL=1)
ELSE(HAS_COOL)
  ADD_DEFINITIONS(-DHAS_NO_COOL=1)
ENDIF(HAS_COOL)

ADD_EXECUTABLE( conversions_container_test_all
  test_driver.cxx

  test_container.cxx
  test_COOL_container.cxx
)

ADD_TEST( conversions_test_container conversions_container_test_all test_container )
ADD_TEST( conversions_test_COOL_container conversions_container_test_all test_COOL_container )

ADD_EXECUTABLE( conversions_container_test_include test_include.cxx )
