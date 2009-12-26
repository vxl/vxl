# brl/bbas/bvgl/tests/CMakeLists.txt

ADD_EXECUTABLE(bvgl_test_all
  test_driver.cxx
  test_bvgl_changes.cxx
)
TARGET_LINK_LIBRARIES(bvgl_test_all bvgl vgl testlib)

ADD_TEST(bvgl_test_changes  ${EXECUTABLE_OUTPUT_PATH}/bvgl_test_all test_bvgl_changes)

ADD_EXECUTABLE( bvgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvgl_test_include bvgl )
