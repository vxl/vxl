USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := brec_pro

SOURCES += brec_prob_map_threshold_process.cxx
SOURCES += brec_change_area_process.cxx
SOURCES += brec_prob_map_roc_compute_process.cxx
SOURCES += brec_prob_map_supress_process.cxx
SOURCES += brec_glitch_overlay_process.cxx
SOURCES += brec_density_to_prob_map_process.cxx
SOURCES += brec_recognize_structure_process.cxx
SOURCES += brec_glitch_process.cxx
SOURCES += brec_bayesian_update_process.cxx
SOURCES += brec_prob_map_area_process.cxx
SOURCES += brec_update_changes_process.cxx
SOURCES += brec_save_mog_image_process.cxx
SOURCES += brec_create_mog_image_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
