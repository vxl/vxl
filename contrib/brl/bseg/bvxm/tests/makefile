USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_apm_processors.cxx
TESTS += test_voxel_grid.cxx
TESTS += test_voxel_storage_disk.cxx
TESTS += test_voxel_storage_mem.cxx
TESTS += test_voxel_world.cxx
TESTS += test_voxel_world_mog_image.cxx
TESTS += test_voxel_world_update.cxx
TESTS += test_lidar_processor.cxx
TESTS += test_voxel_storage_disk_cached.cxx
TESTS += test_platform_computations.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
