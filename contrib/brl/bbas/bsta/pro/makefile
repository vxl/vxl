USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsta_pro
VERSION := 1.0

SOURCES += bsta_register.cxx
MANPAGE_SOURCES += bsta_processes.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
