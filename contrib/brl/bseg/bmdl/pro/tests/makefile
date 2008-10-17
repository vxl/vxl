USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_bmdl_classify_process.cxx
TESTS += test_bmdl_generate_mesh_process.cxx
TESTS += test_bmdl_trace_boundaries_process.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
