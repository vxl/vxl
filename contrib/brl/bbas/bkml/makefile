USES += contrib/brl contrib/brl/bbas core

LIBRARY := bkml
VERSION := 1.0

SOURCES += bkml_write.cxx
SOURCES += bkml_parser.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
