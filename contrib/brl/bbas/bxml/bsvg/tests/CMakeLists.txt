# contrib/brl/bbas/bxml/bsvg/tests/CMakeLists.txt

add_executable( bsvg_test_all
  test_driver.cxx
  test_svg.cxx
)

target_link_libraries( bsvg_test_all bsvg ${VXL_LIB_PREFIX}testlib )

add_test( NAME bsvg_test_svg COMMAND $<TARGET_FILE:bsvg_test_all> test_svg )

add_executable( bsvg_test_include test_include.cxx )
target_link_libraries( bsvg_test_include bsvg )
