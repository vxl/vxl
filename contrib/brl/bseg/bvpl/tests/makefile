USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_subgrid.cxx
TESTS += test_bvpl_kernels.cxx
TESTS += test_neighb_oper.cxx
TESTS += test_bvpl_kernel_functors.cxx
TESTS += test_direction_to_color_map.cxx
TESTS += test_bvpl_vector_operator.cxx
TESTS += test_vector_directions.cxx
TESTS += test_detect_corner.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
