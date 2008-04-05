USES += contrib/brl contrib/brl/bseg core
include ../config.mk
include ${VXLROOT}/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_bvxm_change_detection_display_process.cxx
TESTS += test_bvxm_create_voxel_world_process.cxx
TESTS += test_bvxm_generate_edge_map_process.cxx
TESTS += test_bvxm_illum_index_process.cxx
TESTS += test_bvxm_normalize_image_process.cxx
TESTS += test_bvxm_roi_init_process.cxx
TESTS += test_bvxm_rpc_registration_process.cxx
TESTS += test_bvxm_create_local_rpc_process.cxx
TESTS += test_bvxm_create_normalized_image_process.cxx
TESTS += test_bvxm_create_synth_lidar_data_process.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################
