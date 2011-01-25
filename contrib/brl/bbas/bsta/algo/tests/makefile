USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_fit_weibull.cxx
TESTS += test_gaussian_model.cxx
TESTS += test_mean_shift.cxx
TESTS += test_beta_updater.cxx
TESTS += test_von_mises_update.cxx
TESTS += test_fit_gaussian.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
