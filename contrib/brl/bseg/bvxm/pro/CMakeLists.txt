# This is contrib/brl/bseg/bvxm/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_prodepends on bprb primarily
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/dbil/algo )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})

SET(bvxm_pro_sources
    bvxm_create_voxel_world_process.h       bvxm_create_voxel_world_process.cxx
    bvxm_detect_changes_process.h           bvxm_detect_changes_process.cxx
    bvxm_generate_edge_map_process.h        bvxm_generate_edge_map_process.cxx
    bvxm_render_virtual_view_process.h      bvxm_render_virtual_view_process.cxx
    bvxm_render_expected_image_process.h    bvxm_render_expected_image_process.cxx
    bvxm_change_detection_display_process.h bvxm_change_detection_display_process.cxx
    bvxm_update_process.h                   bvxm_update_process.cxx
    bvxm_normalize_image_process.h          bvxm_normalize_image_process.cxx
    bvxm_illum_index_process.h              bvxm_illum_index_process.cxx
    bvxm_roi_init_process.h                 bvxm_roi_init_process.cxx
    bvxm_rpc_registration_process.h         bvxm_rpc_registration_process.cxx
    bvxm_save_occupancy_raw_process.h       bvxm_save_occupancy_raw_process.cxx
    bvxm_clean_world_process.h              bvxm_clean_world_process.cxx
    bvxm_create_local_rpc_process.h         bvxm_create_local_rpc_process.cxx
    bvxm_create_normalized_image_process.h  bvxm_create_normalized_image_process.cxx

)

AUX_SOURCE_DIRECTORY(Templates bvxm_pro_sources)

ADD_LIBRARY(bvxm_pro ${bvxm_pro_sources})

TARGET_LINK_LIBRARIES(bvxm_pro bvxm bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
ENDIF(EXPAT_FOUND)




