USES += core
include ../config.mk
include ${IUEROOT}/core/testlib/config.mk

TESTS += test_pixels.cxx
# The following tests cannot run in background:
MINI_PROG_SOURCES += test_popup.cxx      #TESTS += test_popup.cxx
MINI_PROG_SOURCES += test_timer.cxx      #TESTS += test_timer.cxx
MINI_PROG_SOURCES += test_events.cxx     #TESTS += test_events.cxx
MINI_PROG_SOURCES += test_front_back.cxx #TESTS += test_front_back.cxx
ifeq (1,$(HAS_X11))
MINI_PROG_SOURCES += test_drawpix_speed.cxx #TESTS += test_drawpix_speed.cxx
MINI_PROG_SOURCES += test_acceleration.cxx  #TESTS += test_acceleration.cxx
endif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${IUEROOT}/config/static-tests.mk
#########################################
