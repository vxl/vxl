USES += contrib/brl contrib/brl/bbas core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_voxel_storage_disk.cxx
TESTS += test_voxel_storage_disk_cached.cxx
TESTS += test_voxel_grid.cxx
TESTS += test_voxel_storage_mem.cxx
TESTS += test_basic_ops.cxx
TESTS += test_grid_to_image_stack.cxx
TESTS += test_bvxm_vrml.cxx

MINI_PROG_SOURCES += test_include.cxx
MINI_PROG_SOURCES += test_template_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
