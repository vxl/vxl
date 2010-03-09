USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_bsta_histogram.cxx
TESTS += test_k_medoid.cxx
TESTS += test_otsu_threshold.cxx
TESTS += test_gaussian_full.cxx
TESTS += test_gaussian_indep.cxx
TESTS += test_gaussian_sphere.cxx
TESTS += test_mixture.cxx
TESTS += test_weibull.cxx
TESTS += test_sampler.cxx
TESTS += test_parzen_sphere.cxx
TESTS += test_beta.cxx
TESTS += test_von_mises.cxx
TESTS += test_kent.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
