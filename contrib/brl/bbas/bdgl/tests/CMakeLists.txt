# brl/bbas/bdgl/tests/CMakeLists.txt

LINK_LIBRARIES( bdgl vdgl vtol vnl testlib )

ADD_EXECUTABLE(bdgl_test_all  test_curve_algs.cxx)
ADD_TEST(      bdgl_test_curve_algs bdgl_test_all)

ADD_EXECUTABLE( bdgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bdgl_test_include bdgl )
