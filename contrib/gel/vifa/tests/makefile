USES += contrib/gel contrib/brl contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk
include ${VXLROOT}/contrib/brl/bseg/sdet/config.mk

TESTMAIN := test_driver.cxx
SOURCES += test_ifa_factory.cxx

TESTS += test_coll_lines_params.cxx
TESTS += test_group_pgram_params.cxx
TESTS += test_image_histogram.cxx
PARAMS_test_image_histogram := cam0013.tif
TESTS += test_int_face_attr.cxx
PARAMS_test_int_face_attr := testimg.tif
TESTS += test_int_face_attr_common_params.cxx
TESTS += test_int_faces_adj_attr.cxx
PARAMS_test_int_faces_adj_attr := testimg.tif
TESTS += test_int_faces_attr.cxx
PARAMS_test_int_faces_attr := testimg.tif
TESTS += test_norm_params.cxx
TESTS += test_parallel_params.cxx
TESTS += test_region_proc.cxx
PARAMS_test_region_proc := testimg.tif

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
