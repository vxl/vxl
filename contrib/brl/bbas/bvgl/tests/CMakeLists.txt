# brl/bbas/bvgl/tests/CMakeLists.txt

ADD_EXECUTABLE(bvgl_test_all
  test_driver.cxx
  test_articulated_poly.cxx
)
TARGET_LINK_LIBRARIES(bvgl_test_all bvgl vgl vnl testlib)


