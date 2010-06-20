# contrib/brl/bbas/bsta/vis/tests/CMakeLists.txt

ADD_EXECUTABLE( bsta_vis_test_all
  test_driver.cxx
  test_vis_svg.cxx
)

TARGET_LINK_LIBRARIES( bsta_vis_test_all bsta_vis testlib )

ADD_TEST( bsta_vis_test_vis_svg  ${EXECUTABLE_OUTPUT_PATH}/bsta_vis_test_all test_vis_svg )

ADD_EXECUTABLE( bsta_vis_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsta_vis_test_include bsta_vis )
