USES += contrib/cul contrib/gel core
include ../config.mk
include ${VXLROOT}/contrib/gel/mrc/vpgl/algo/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_tracks.cxx
TESTS += test_propose_matches.cxx
TESTS += test_bundler_utils.cxx
TESTS += test_match_ann.cxx
TESTS += test_refine.cxx
TESTS += test_tracks_detect.cxx
TESTS += test_initial_recon.cxx
TESTS += test_pipeline.cxx
TESTS += test_ply.cxx
TESTS += test_add_next_image.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
