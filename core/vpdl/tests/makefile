USES += core
include ../config.mk
include ${VXLROOT}/core/vnl/algo/config.mk
include ${VXLROOT}/core/vnl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN = test_driver.cxx
TESTS += test_gaussian_sphere.cxx
TESTS += test_gaussian.cxx
TESTS += test_gaussian_indep.cxx
TESTS += test_eigen_sym_matrix.cxx
TESTS += test_mixture_of.cxx
TESTS += test_update_gaussian.cxx
TESTS += test_kernel_gaussian_sfbw.cxx
TESTS += test_mixture.cxx
TESTS += test_update_mog.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
