USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bvpgl
VERSION := 1.0

MANPAGE_SOURCES += bvpgl_camera_estimator.txx
MANPAGE_SOURCES += bvpgl_camera_estimator_amoeba.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
