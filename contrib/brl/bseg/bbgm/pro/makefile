USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bbgm_pro
VERSION := 1.0

SOURCES += bbgm_display_dist_image_process.cxx
SOURCES += bbgm_load_image_of_process.cxx
SOURCES += bbgm_measure_process.cxx
SOURCES += bbgm_save_image_of_process.cxx
SOURCES += bbgm_update_dist_image_process.cxx
SOURCES += bbgm_update_dist_image_stream_process.cxx
SOURCES += bbgm_local_frame_trans_process.cxx
SOURCES += bbgm_update_parzen_dist_image_process.cxx
SOURCES += bbgm_register.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
