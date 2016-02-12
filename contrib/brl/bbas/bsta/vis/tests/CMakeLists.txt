# contrib/brl/bbas/bsta/vis/tests/CMakeLists.txt

add_executable( bsta_vis_test_all
  test_driver.cxx
  test_vis_svg.cxx
)

target_link_libraries( bsta_vis_test_all bsta_vis ${VXL_LIB_PREFIX}testlib )

add_test( NAME bsta_vis_test_vis_svg COMMAND $<TARGET_FILE:bsta_vis_test_all> test_vis_svg )

add_executable( bsta_vis_test_include test_include.cxx )
target_link_libraries( bsta_vis_test_include bsta_vis )
