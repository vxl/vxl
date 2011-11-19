USES += core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_camera_io.cxx
TESTS += test_local_rational_camera_io.cxx
TESTS += test_rational_camera_io.cxx
TESTS += test_proj_camera_io.cxx
TESTS += test_perspective_camera_io.cxx
TESTS += test_lvcs_io.cxx
TESTS += test_affine_camera_io.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
