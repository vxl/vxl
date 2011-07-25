USES += contrib/brl contrib/brl/bbas core

LIBRARY := brad_pro
VERSION := 1.0

SOURCES += brad_register.cxx
MANPAGE_SOURCES += brad_processes.h

SOURCES += processes/brad_estimate_shadow_model_process.cxx
SOURCES += processes/brad_estimate_phongs_model_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
