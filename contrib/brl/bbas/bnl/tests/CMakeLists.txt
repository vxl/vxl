# brl/bbas/bnl/tests/CMakeLists.txt
# Tests for bnl library

ADD_EXECUTABLE( bnl_test_all
  test_driver.cxx
)

TARGET_LINK_LIBRARIES( bnl_test_all bnl testlib )


ADD_EXECUTABLE( bnl_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bnl_test_include bnl )
