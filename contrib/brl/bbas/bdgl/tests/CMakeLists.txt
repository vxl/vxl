# brl/bbas/bdgl/tests/CMakeLists.txt

LINK_LIBRARIES( bdgl vdgl vtol vnl )

ADD_EXECUTABLE(bdgl_test_curve_algs      test_curve_algs.cxx)
ADD_TEST(      bdgl_test_curve_algs bdgl_test_curve_algs)
