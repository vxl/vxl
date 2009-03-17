USES += contrib/brl contrib/brl/bpro contrib/brl/bpro/core contrib/brl/bpro/core/vil_pro core
include ../config.mk
include ${VXLROOT}/contrib/brl/bpro/bprb/config.mk
include ${VXLROOT}/core/vil/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx

TESTS += test_vil_convert_to_n_planes_process.cxx
TESTS += test_vil_crop_image_process.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
