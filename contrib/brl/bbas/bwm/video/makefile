USES += contrib/brl contrib/brl/bbas core

LIBRARY := video
VERSION := 1.0

SOURCES += bwm_video_cam_istream.cxx
MANPAGE_SOURCES += bwm_video_cam_istream_sptr.h
SOURCES += bwm_video_cam_ostream.cxx
MANPAGE_SOURCES += bwm_video_cam_ostream_sptr.h
SOURCES += bwm_video_corr.cxx
SOURCES += bwm_video_corr_processor.cxx
MANPAGE_SOURCES += bwm_video_corr_sptr.h
SOURCES += bwm_video_registration.cxx
SOURCES += bwm_video_site_io.cxx
MANPAGE_SOURCES += bwm_video_site_io_defs.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests exe
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
