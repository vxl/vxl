USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_project.cxx
TESTS += test_camera_from_box.cxx
TESTS += test_construct_cameras.cxx
TESTS += test_fm_compute.cxx
TESTS += test_interpolate.cxx
TESTS += test_optimize_camera.cxx
TESTS += test_camera_homographies.cxx
TESTS += test_lens_warp_mapper.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
