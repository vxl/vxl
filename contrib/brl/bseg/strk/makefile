USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core

LIBRARY := strk
VERSION := 1.0

MANPAGE_SOURCES += strk_art_info_model.h
MANPAGE_SOURCES += strk_art_info_model_sptr.h
SOURCES += strk_art_model_display_process.cxx
SOURCES += strk_art_info_model.cxx
SOURCES += strk_corr_tracker_process.cxx
SOURCES += strk_info_model_tracker.cxx
SOURCES += strk_info_model_tracker_params.cxx
SOURCES += strk_info_model_tracker_process.cxx
SOURCES += strk_info_tracker.cxx
SOURCES += strk_info_tracker_params.cxx
SOURCES += strk_info_tracker_process.cxx
SOURCES += strk_track_display_process.cxx
SOURCES += strk_tracker.cxx
SOURCES += strk_tracker_params.cxx
SOURCES += strk_tracking_face_2d.cxx
MANPAGE_SOURCES += strk_tracking_face_2d_sptr.h
SOURCES += strk_epi_point.cxx
MANPAGE_SOURCES += strk_epi_point_sptr.h
SOURCES += strk_epi_seg.cxx
MANPAGE_SOURCES += strk_epi_seg_sptr.h
SOURCES += strk_epipolar_grouper.cxx
SOURCES += strk_epipolar_grouper_params.cxx
SOURCES += strk_epipolar_grouper_process.cxx
SOURCES += strk_region_info.cxx
SOURCES += strk_region_info_params.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
