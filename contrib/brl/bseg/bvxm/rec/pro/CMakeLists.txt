# This is contrib/dbvxm/pro/CMakeLists.txt
# dbvxm's Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/dbil/algo )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )

SET(bvxm_rec_pro_sources
    bvxm_rec_update_changes_process.h  bvxm_rec_update_changes_process.cxx
    bvxm_rec_density_to_prob_map_process.h  bvxm_rec_density_to_prob_map_process.cxx
    bvxm_prob_map_roc_compute_process.h         bvxm_prob_map_roc_compute_process.cxx
    bvxm_prob_map_threshold_process.h         bvxm_prob_map_threshold_process.cxx
    bvxm_rec_structure_process.h  bvxm_rec_structure_process.cxx
    bvxm_prob_map_supress_process.h         bvxm_prob_map_supress_process.cxx
    bvxm_prob_map_area_process.h         bvxm_prob_map_area_process.cxx
    bvxm_rec_update_changes_wrt_area_process.h  bvxm_rec_update_changes_wrt_area_process.cxx
    bvxm_change_area_process.h         bvxm_change_area_process.cxx
)

ADD_LIBRARY(bvxm_rec_pro ${bvxm_rec_pro_sources})

TARGET_LINK_LIBRARIES(bvxm_rec_pro bvxm_rec bvxm bvxm_io bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )




