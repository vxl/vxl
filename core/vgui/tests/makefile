USES += vxl oxl
TRY_X11 := 1
TRY_OPENGL := 1

###############################################
include ${IUELOCALROOT}/oxl/vgui/easy-client.mk
###############################################

TESTS += test_popup.cxx
TESTS += test_timer.cxx
TESTS += test_events.cxx
TESTS += test_front_back.cxx
TESTS += test_pixels.cxx
ifeq (1,$(HAS_X11))
TESTS += test_drawpix_speed.cxx
TESTS += test_acceleration.cxx
endif

# Don't compile these by default.
ifdef EXTRA_TESTS
SUBDIRS += old-tests Fl gtk Xm
endif

MINI_PROG_SOURCES := $(TESTS)

######################################
include ${IUEROOT}/config/top-rules.mk
#include ${IUEROOT}/config/static-tests.mk
# (cannot do that since these tests cannot run in background)
##########################################
