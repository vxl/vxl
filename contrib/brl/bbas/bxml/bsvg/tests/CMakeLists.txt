# contrib/brl/bbas/bxml/bsvg/tests/CMakeLists.txt

ADD_EXECUTABLE( bsvg_test_all
  test_driver.cxx
  test_svg.cxx
)

TARGET_LINK_LIBRARIES( bsvg_test_all bsvg testlib )

ADD_TEST( bsvg_test_svg  ${EXECUTABLE_OUTPUT_PATH}/bsvg_test_all test_svg )

ADD_EXECUTABLE( bsvg_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsvg_test_include bsvg )
