USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ../algo/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

SOURCES += test_share.cxx
TESTS += test_imls_surface.cxx
TESTS += test_intersect.cxx
TESTS += test_kd_tree.cxx
TESTS += test_detect.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
