USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += bvgl_test_h_matrix_1d.cxx
TESTS += bvgl_test_h_matrix_2d.cxx
TESTS += bvgl_test_h_matrix_3d.cxx
TESTS += bvgl_test_h_matrix_2d_compute_linear.cxx
TESTS += bvgl_test_h_matrix_2d_compute_4point.cxx
TESTS += bvgl_test_norm_trans_2d.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
