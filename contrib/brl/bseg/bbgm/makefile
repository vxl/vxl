USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bbgm
VERSION := 1.0

SOURCES += bbgm_viewer.cxx
SOURCES += bbgm_image_of.cxx
MANPAGE_SOURCES += bbgm_apply.h
MANPAGE_SOURCES += bbgm_detect.h
MANPAGE_SOURCES += bbgm_image_sptr.h
MANPAGE_SOURCES += bbgm_planes_to_sample.h
MANPAGE_SOURCES += bbgm_update.h
MANPAGE_SOURCES += bbgm_image_of.txx
MANPAGE_SOURCES += bbgm_viewer_sptr.h
SOURCES += bbgm_loader.cxx
MANPAGE_SOURCES += bbgm_measure.h
MANPAGE_SOURCES += bbgm_view_maker.h
MANPAGE_SOURCES += bbgm_view_maker_sptr.h

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
