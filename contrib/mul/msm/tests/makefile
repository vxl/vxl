USES += contrib/mul core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_points.cxx
TESTS += test_shape_model.cxx
TESTS += test_param_limiters.cxx
TESTS += test_wt_mat_2d.cxx
TESTS += test_curves.cxx
TESTS += test_aligners.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
