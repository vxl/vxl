USES += contrib/brl contrib/brl/bbas contrib/brl/bseg contrib/brl/bmvl core
include ../config.mk
include ${VXLROOT}/contrib/brl/bbas/bgui/config.mk
include ${VXLROOT}/core/vgui/config.mk
include ${VXLROOT}/core/testlib/config.mk

LIBRARY := brct_test
SOURCES += brct_windows_frame.cxx
SOURCES += brct_menus.cxx

IULIBS += -lbrct_test
MINI_PROG_SOURCES += test_brct_kalman.cxx

TESTMAIN := test_driver.cxx
TESTS += brct_test_synthetic_data.cxx
TESTS += brct_test_compute_P.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
