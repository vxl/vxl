USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bvpl_octree_pro
VERSION := 1.0

MANPAGE_SOURCES += bvpl_octree_processes.h
SOURCES += bvpl_octree_register.cxx

SOURCES += processes/bvpl_save_vrml_process.cxx
SOURCES += processes/bvpl_scene_vector_operator_process.cxx
SOURCES += processes/bvpl_create_scene_process.cxx
SOURCES += processes/bvpl_scene_kernel_operator_process.cxx
SOURCES += processes/bvpl_plane_propagate_process.cxx
SOURCES += processes/bvpl_nonmax_supp_process.cxx
SOURCES += processes/bvpl_compute_gauss_gradients.cxx
SOURCES += processes/bvpl_discover_pca_features_process.cxx
SOURCES += processes/bvpl_compute_pca_test_error_process.cxx
SOURCES += processes/bvpl_block_kernel_operator_process.cxx
SOURCES += processes/bvpl_add_taylor_errors_process.cxx
SOURCES += processes/bvpl_add_pca_errors_process.cxx
SOURCES += processes/bvpl_load_pca_error_scenes_process.cxx
SOURCES += processes/bvpl_compute_taylor_error_process.cxx
SOURCES += processes/bvpl_pca_project_process.cxx
SOURCES += processes/bvpl_load_taylor_scenes_process.cxx
SOURCES += processes/bvpl_block_avg_value_process.cxx
SOURCES += processes/bvpl_scene_histogram_process.cxx
SOURCES += processes/bvpl_compute_pca_error_block_process.cxx
SOURCES += processes/bvpl_compute_pca_error_scene_process.cxx
SOURCES += processes/bvpl_load_pca_info_process.cxx
SOURCES += processes/bvpl_init_global_pca_process.cxx
SOURCES += processes/bvpl_pca_global_statistics_process.cxx
SOURCES += processes/bvpl_global_pca_process.cxx
SOURCES += processes/bvpl_normalize_pca_training_error_process.cxx
SOURCES += processes/bvpl_combine_pairwise_statistics_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
