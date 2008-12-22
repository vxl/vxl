# This is contrib/brl/bseg/brec/pro/CMakeLists.txt
# Brown Recognition Process Library

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/dbil/algo )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc/vpgl )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )

SET(brec_pro_sources
    brec_update_changes_process.h  brec_update_changes_process.cxx
    brec_density_to_prob_map_process.h  brec_density_to_prob_map_process.cxx
    brec_prob_map_roc_compute_process.h         brec_prob_map_roc_compute_process.cxx
    brec_prob_map_threshold_process.h         brec_prob_map_threshold_process.cxx
    brec_recognize_structure_process.h  brec_recognize_structure_process.cxx
    brec_prob_map_supress_process.h         brec_prob_map_supress_process.cxx
    brec_prob_map_area_process.h         brec_prob_map_area_process.cxx
    brec_change_area_process.h         brec_change_area_process.cxx
    brec_bayesian_update_process.h  brec_bayesian_update_process.cxx
    brec_glitch_process.h  brec_glitch_process.cxx
    brec_glitch_overlay_process.h  brec_glitch_overlay_process.cxx
    brec_save_mog_image_process.h         brec_save_mog_image_process.cxx
    brec_create_mog_image_process.h       brec_create_mog_image_process.cxx
)

ADD_LIBRARY(brec_pro ${brec_pro_sources})

TARGET_LINK_LIBRARIES(brec_pro brec bvxm bvxm_io bbgm_pro bprb bil_algo sdet vpgl vpgl_io vpgl_algo vpgl_file_formats vpgl_pro vsol vil vil_io vil_pro vnl vnl_io bgeo vgl vgl_algo brip brdb)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

