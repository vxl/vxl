USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${IUEROOT}/contrib/brl/bbas/bgui/config.mk
include ${IUEROOT}/core/vgui/config.mk

LIBRARY := brct_test
SOURCES += brct_windows_frame.cxx
SOURCES += brct_menus.cxx

IULIBS += -lbrct_test
#TESTS += test_brct_kalman.cxx
MINI_PROG_SOURCES += test_brct_kalman.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
