# contrib/brl/bbas/bxml/bsvg/tests/CMakeLists.txt

ADD_EXECUTABLE( bsvg_test_all
  test_driver.cxx
  test_svg.cxx
)

TARGET_LINK_LIBRARIES( bsvg_test_all bsvg testlib )

ADD_TEST( NAME bsvg_test_svg COMMAND $<TARGET_FILE:bsvg_test_all> test_svg )

ADD_EXECUTABLE( bsvg_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsvg_test_include bsvg )
