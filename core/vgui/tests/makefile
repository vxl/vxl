USES += vxl oxl
TRY_X11 := 1

###############################################
include ${IUELOCALROOT}/oxl/vgui/easy-client.mk
###############################################

MINI_PROG_SOURCES += test_popup.cxx
MINI_PROG_SOURCES += test_timer.cxx
MINI_PROG_SOURCES += test_events.cxx
MINI_PROG_SOURCES += test_front_back.cxx
MINI_PROG_SOURCES += test_pixels.cxx
ifeq (1,$(HAS_X11))
MINI_PROG_SOURCES += test_drawpix_speed.cxx
MINI_PROG_SOURCES += test_acceleration.cxx
endif

# Don't compile these by default.
ifdef EXTRA_TESTS
SUBDIRS += old-tests Fl gtk Xm
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
