USES += contrib/mul core
include ../config.mk
include ${VXLROOT}/contrib/mul/vil3d/config.mk
include ${VXLROOT}/contrib/mul/mbl/config.mk
include ${VXLROOT}/core/vgl/config.mk
include ${VXLROOT}/core/vnl/config.mk
include ${VXLROOT}/core/vul/config.mk
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
TESTS += test_image_bounds_and_centre_3d.cxx
TESTS += test_save.cxx
TESTS += test_reflect.cxx
TESTS += test_run_length_encoding.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
