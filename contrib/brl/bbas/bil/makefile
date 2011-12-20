USES += contrib/brl contrib/brl/bbas core

LIBRARY := bil
VERSION := 1.0

MANPAGE_SOURCES += bil_bounded_image_view.txx
MANPAGE_SOURCES += bil_math.h
SOURCES += bil_raw_image_istream.cxx

SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
