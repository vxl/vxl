# brl/bbas/bdgl/tests/CMakeLists.txt

INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR} )

LINK_LIBRARIES( bdgl vdgl vtol vnl testlib )

ADD_EXECUTABLE(bdgl_test_all
	       test_curve_algs.cxx
	       test_region_algs.cxx)

ADD_TEST(test_curve_algs bdgl_test_all test_curve_algs)
ADD_TEST(test_region_algs bdgl_test_all test_region_algs)

ADD_EXECUTABLE( bdgl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bdgl_test_include bdgl )
