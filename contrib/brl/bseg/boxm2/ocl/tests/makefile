USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += boxm2_ocl_test_utils.cxx

TESTS += test_refine.cxx
TESTS += test_image_pyramid.cxx
TESTS += test_process_mains.cxx
TESTS += test_weighted_em.cxx
TESTS += test_kernel_vector_filter.cxx
TESTS += test_kernel_filter.cxx
TESTS += test_ocl_cholesky.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
