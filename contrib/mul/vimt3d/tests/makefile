USES += contrib/mul core
include ../config.mk
include ${VXLROOT}/core/vpl/config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_transform_3d.cxx
TESTS += test_image_3d_of.cxx
TESTS += test_load.cxx
TESTS += test_sample_grid_trilin.cxx
PARAMS_test_load := ../../vil3d/tests/file_read_data
TESTS += test_v3i.cxx
TESTS += test_from_image_2d.cxx
TESTS += test_resample.cxx
TESTS += test_gauss_reduce.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
