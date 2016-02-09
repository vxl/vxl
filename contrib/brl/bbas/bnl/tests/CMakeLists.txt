# brl/bbas/bnl/tests/CMakeLists.txt
# Tests for bnl library

add_executable( bnl_test_all
  test_driver.cxx
)

target_link_libraries( bnl_test_all bnl ${VXL_LIB_PREFIX}testlib )
add_executable( bnl_test_include test_include.cxx )
target_link_libraries( bnl_test_include bnl )
