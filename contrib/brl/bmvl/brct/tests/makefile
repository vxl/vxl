USES += contrib/brl contrib/brl/bbas contrib/brl/bseg contrib/brl/bmvl core
include ../config.mk
include ${IUEROOT}/contrib/brl/bbas/bgui/config.mk
include ${IUEROOT}/core/vgui/config.mk

LIBRARY := brct_test
SOURCES += brct_windows_frame.cxx
SOURCES += brct_menus.cxx

IULIBS += -lbrct_test
MINI_PROG_SOURCES += test_brct_kalman.cxx
TESTS += brct_test_synthetic_data.cxx
TESTS += brct_test_compute_P.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
