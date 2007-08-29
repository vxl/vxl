USES += contrib/gel core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_camera_compute.cxx
TESTS += test_construct_cameras.cxx
TESTS += test_fm_compute.cxx
TESTS += test_lens_warp_mapper.cxx
TESTS += test_optimize_camera.cxx
TESTS += test_project.cxx
TESTS += test_backproject.cxx
TESTS += test_ortho_procrustes.cxx
TESTS += test_bundle_adjust.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
