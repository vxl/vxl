USES += core
include ../config.mk

MINI_PROG_SOURCES += test_popup.cxx      #TESTS += test_popup.cxx
MINI_PROG_SOURCES += test_timer.cxx      #TESTS += test_timer.cxx
MINI_PROG_SOURCES += test_events.cxx     #TESTS += test_events.cxx
MINI_PROG_SOURCES += test_front_back.cxx #TESTS += test_front_back.cxx
TESTS += test_pixels.cxx
ifeq (1,$(HAS_X11))
MINI_PROG_SOURCES += test_drawpix_speed.cxx #TESTS += test_drawpix_speed.cxx
MINI_PROG_SOURCES += test_acceleration.cxx  #TESTS += test_acceleration.cxx
endif

######################################
include ${IUEROOT}/config/top-rules.mk
#include ${IUEROOT}/config/static-tests.mk
# (cannot do that since these tests cannot run in background)
##########################################
