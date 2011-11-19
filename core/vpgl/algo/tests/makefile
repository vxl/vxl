USES += core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_ortho_procrustes.cxx
TESTS += test_camera_compute.cxx
TESTS += test_bundle_adjust.cxx
TESTS += test_ba_fixed_k_lsqr.cxx
TESTS += test_fm_compute.cxx
TESTS += test_em_compute_5_point.cxx
TESTS += test_optimize_camera.cxx
TESTS += test_triangulate.cxx
TESTS += test_ray.cxx
TESTS += test_rational_adjust_onept.cxx
TESTS += test_ray_intersect.cxx
TESTS += test_backproject.cxx
TESTS += test_ba_shared_k_lsqr.cxx
TESTS += test_camera_convert.cxx
TESTS += test_rational_adjust.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
