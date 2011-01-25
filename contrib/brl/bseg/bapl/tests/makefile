USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_matcher.cxx
TESTS += test_transform.cxx
TESTS += test_compute_tracks.cxx
TESTS += test_lowe_pyramid_set.cxx
TESTS += test_match_keypoints.cxx
TESTS += test_roi.cxx
TESTS += test_dense_sift.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
