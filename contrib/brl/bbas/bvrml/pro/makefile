USES += contrib/brl contrib/brl/bbas core

LIBRARY := bvrml_pro
VERSION := 1.0

SOURCES += bvrml_register.cxx
MANPAGE_SOURCES += bvrml_processes.h

SOURCES += processes/bvrml_initialize_process.cxx
SOURCES += processes/bvrml_write_processes.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
