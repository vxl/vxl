# brl/bbas/bvgl/tests/CMakeLists.txt

ADD_EXECUTABLE(bvgl_test_all
  test_driver.cxx
  test_articulated_poly.cxx
  test_misc.cxx
)
TARGET_LINK_LIBRARIES(bvgl_test_all bvgl vgl testlib)

ADD_TEST(bvgl_test_articulated_poly  ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_articulated_poly)
ADD_TEST(bvgl_test_misc              ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_misc)

ADD_EXECUTABLE( bvgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvgl_test_include bvgl )
