USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsta_pro
VERSION := 1.0

SOURCES += bsta_register.cxx
MANPAGE_SOURCES += bsta_processes.h

SOURCES += processes/bsta_sigma_normalization_table_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
