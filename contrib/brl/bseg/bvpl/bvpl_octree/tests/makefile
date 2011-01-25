USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
SOURCES += test_utils.cxx

TESTS += test_octree_kernel_operator.cxx
TESTS += test_octree_neighbors.cxx
TESTS += test_pca_kernels.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
