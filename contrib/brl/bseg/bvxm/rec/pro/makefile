USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvxm_rec_pro

SOURCES += bvxm_change_area_process.cxx
SOURCES += bvxm_prob_map_area_process.cxx
SOURCES += bvxm_prob_map_roc_compute_process.cxx
SOURCES += bvxm_prob_map_supress_process.cxx
SOURCES += bvxm_prob_map_threshold_process.cxx
SOURCES += bvxm_rec_density_to_prob_map_process.cxx
SOURCES += bvxm_rec_structure_process.cxx
SOURCES += bvxm_rec_update_changes_process.cxx
SOURCES += bvxm_rec_update_changes_wrt_area_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
