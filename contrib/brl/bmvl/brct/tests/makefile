USES += contrib/brl contrib/brl/bbas contrib/gel contrib/oxl core
include ../config.mk
include ${IUEROOT}/core/vgui/config.mk

LIBRARY := bcrt_test
SOURCES += brct_windows_frame.cxx
SOURCES += brct_menus.cxx

IULIBS += -lbcrt_test
#TESTS += test_brct_kalman.cxx
MINI_PROG_SOURCES += test_brct_kalman.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
