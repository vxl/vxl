USES += core
include ../config.mk
include ${VXLROOT}/core/vul/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_pixels.cxx
TESTS += test_displaybase.cxx
#TESTS += test_generic_image_view.cxx
TESTS += test_image_tableau.cxx
PARAMS_test_image_tableau = test_image.ppm
TESTS += test_range_map.cxx
# The following tests cannot run in background:
MINI_PROG_SOURCES += test_popup.cxx      #TESTS += test_popup.cxx
MINI_PROG_SOURCES += test_timer.cxx      #TESTS += test_timer.cxx
MINI_PROG_SOURCES += test_events.cxx     #TESTS += test_events.cxx
MINI_PROG_SOURCES += test_front_back.cxx #TESTS += test_front_back.cxx
ifeq (1,$(HAS_X11))
MINI_PROG_SOURCES += test_drawpix_speed.cxx #TESTS += test_drawpix_speed.cxx
MINI_PROG_SOURCES += test_drawpix_speed2.cxx#TESTS += test_drawpix_speed2.cxx
MINI_PROG_SOURCES += test_acceleration.cxx  #TESTS += test_acceleration.cxx
endif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
