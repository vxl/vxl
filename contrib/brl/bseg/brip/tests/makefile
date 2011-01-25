USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_histogram.cxx
TESTS += test_mutual_info.cxx
TESTS += test_watershed.cxx
TESTS += test_fourier.cxx
TESTS += test_homography.cxx
TESTS += test_lucas_kanade.cxx
TESTS += test_Horn_Schunck.cxx
TESTS += test_max_scale_response.cxx
TESTS += test_quadtree.cxx
TESTS += test_label_equivalence.cxx
TESTS += test_mask.cxx
TESTS += test_extrema.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
