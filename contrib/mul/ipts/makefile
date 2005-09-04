USES += contrib/mul core

LIBRARY := ipts
VERSION := 1.0

SOURCES += ipts_local_entropy.cxx
SOURCES += ipts_corner_pyramid.cxx
SOURCES += ipts_entropy_pyramid.cxx
SOURCES += ipts_orientation_pyramid.cxx
MANPAGE_SOURCES += ipts_scale_space_peaks.h
MANPAGE_SOURCES += ipts_draw.h

SUBDIRS += tools

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
