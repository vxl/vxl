USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := brip_pro
VERSION := 1.0

SOURCES += brip_register.cxx

MANPAGE_SOURCES += brip_processes.h
SOURCES += processes/brip_extrema_process.cxx
SOURCES += processes/brip_blobwise_mutual_info_process.cxx
SOURCES += processes/brip_blobwise_kl_div_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
