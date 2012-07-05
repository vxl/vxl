# bbas/bcvr/tests/CMakeLists.txt

#INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

ADD_EXECUTABLE( bcvr_test_all
  test_driver.cxx
  test_cvmatch.cxx
  test_clsd_cvmatch.cxx
)
TARGET_LINK_LIBRARIES( bcvr_test_all bcvr bsol testlib)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/line1.con  ${CMAKE_CURRENT_BINARY_DIR}/line1.con COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/line2.con  ${CMAKE_CURRENT_BINARY_DIR}/line2.con COPYONLY )

ADD_TEST( bcvr_cvmatch              bcvr_test_all test_cvmatch )
ADD_TEST( bcvr_clsd_cvmatch         bcvr_test_all test_clsd_cvmatch )

ADD_EXECUTABLE( bcvr_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bcvr_test_include bcvr )
