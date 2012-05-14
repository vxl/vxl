USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsl
VERSION := 1.0

MANPAGE_SOURCES += bsl_opinion.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
SOURCES += dummy.cxx
