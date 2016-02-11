# bbas/bcvr/tests/CMakeLists.txt

#include_directories(${GEL_INCLUDE_DIR} )

add_executable( bcvr_test_all
  test_driver.cxx
  test_cvmatch.cxx
  test_clsd_cvmatch.cxx
)
target_link_libraries( bcvr_test_all bcvr bsol ${VXL_LIB_PREFIX}testlib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/line1.con  ${CMAKE_CURRENT_BINARY_DIR}/line1.con COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/line2.con  ${CMAKE_CURRENT_BINARY_DIR}/line2.con COPYONLY )

add_test( NAME bcvr_cvmatch      COMMAND $<TARGET_FILE:bcvr_test_all> test_cvmatch )
add_test( NAME bcvr_clsd_cvmatch COMMAND $<TARGET_FILE:bcvr_test_all> test_clsd_cvmatch )

add_executable( bcvr_test_include test_include.cxx )
target_link_libraries( bcvr_test_include bcvr )
