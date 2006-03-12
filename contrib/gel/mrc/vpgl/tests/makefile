USES += contrib/gel core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_affine_camera.cxx
TESTS += test_calibration_matrix.cxx
TESTS += test_camera_io.cxx
TESTS += test_fundamental_matrix.cxx
TESTS += test_perspective_camera.cxx
TESTS += test_poly_radial_distortion.cxx
TESTS += test_proj_camera.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
