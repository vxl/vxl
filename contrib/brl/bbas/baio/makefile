USES += contrib/brl contrib/brl/bbas core

LIBRARY := baio
VERSION := 1.0

SOURCES += baio_unix.cxx
MANPAGE_SOURCES += baio.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
