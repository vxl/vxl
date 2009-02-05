USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := brec_pro

SOURCES += brec_register.cxx

MANPAGE_SOURCES += brec_processes.h
SOURCES += processes/brec_prob_map_threshold_process.cxx
SOURCES += processes/brec_change_area_process.cxx
SOURCES += processes/brec_prob_map_roc_compute_process.cxx
SOURCES += processes/brec_prob_map_supress_process.cxx
SOURCES += processes/brec_glitch_overlay_process.cxx
SOURCES += processes/brec_density_to_prob_map_process.cxx
SOURCES += processes/brec_recognize_structure_process.cxx
SOURCES += processes/brec_glitch_process.cxx
SOURCES += processes/brec_bayesian_update_process.cxx
SOURCES += processes/brec_prob_map_area_process.cxx
SOURCES += processes/brec_update_changes_process.cxx
SOURCES += processes/brec_create_mog_image_process.cxx
SOURCES += processes/brec_construct_op_models_processes.cxx
SOURCES += processes/brec_learner_processes.cxx
SOURCES += processes/brec_hierarchy_processes.cxx
SOURCES += processes/brec_learner_update_processes.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
