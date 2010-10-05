# VRML IO library

USES += core contrib/oxl

LIBRARY := vrml
VERSION := 1.0

IMMEDIATE_LIBS += -lvul

SOURCES += vrml_out.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
